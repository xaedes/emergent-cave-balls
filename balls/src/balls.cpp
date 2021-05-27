#include "balls/balls.h"

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace balls {
    
    void Balls::parameterUpdate()
    {
        glfwSwapInterval(params.vsync ? 1 : 0);
        resizeBalls(m_ballsProgram.num_items.get());
    }

    void Balls::setup(gui_application::Random& random)
    {
        m_random = &random;
        auto width = params.width;
        auto height = params.height;

        m_ballsProgram.setup(width, height);
        m_renderProgram.setup();
        
        // m_texturedRenderProgram = createTexturedRenderProgram();
        // checkGLError();
        // m_texturedRenderProgram.getShaders()[0].setup();
        // m_texturedRenderProgram.getShaders()[1].setup();
        // m_texturedRenderProgram.setup();
        // checkGLError();
        int num_items = 100;
        // int num_items = 1024 * 1024;
        // int num_items = 1024 * 10;
        // int num_items = 1024;
        m_ballsProgram.num_items.set(num_items);

        for (int i=0; i<2; ++i)
        {
            m_balls[i] = HostDeviceBuffer<Ball>(GL_SHADER_STORAGE_BUFFER, GL_DYNAMIC_DRAW, num_items, num_items);
            m_balls[i].init();

            m_toprocess[i] = HostDeviceBuffer<int32_t>(GL_SHADER_STORAGE_BUFFER, GL_DYNAMIC_DRAW, num_items, num_items);
            m_toprocess[i].init();
        }
        m_ll_head = HostDeviceBuffer<int32_t>(GL_SHADER_STORAGE_BUFFER, GL_DYNAMIC_DRAW, width * height, width * height);
        m_ll_head.init();
        
        m_ll_count = HostDeviceBuffer<int32_t>(GL_SHADER_STORAGE_BUFFER, GL_DYNAMIC_DRAW, width * height, width * height);
        m_ll_count.init();
        m_ll_next = HostDeviceBuffer<int32_t>(GL_SHADER_STORAGE_BUFFER, GL_DYNAMIC_DRAW, num_items, num_items);
        m_ll_next.init();
        m_counters = HostDeviceBuffer<int32_t>(GL_SHADER_STORAGE_BUFFER, GL_DYNAMIC_DRAW, 2, 2);
        m_counters.init();

        m_neighbor_iterators = HostDeviceBuffer<glm::ivec4>(GL_SHADER_STORAGE_BUFFER, GL_DYNAMIC_DRAW, num_items, num_items);
        m_neighbor_iterators.init();
          

        m_ballVertices = HostDeviceBuffer<glm::vec4>(GL_SHADER_STORAGE_BUFFER, GL_DYNAMIC_DRAW, 1+16, 1+16);
        m_ballVertices.init();
        createBallVertices();
        m_ballVertices.upload();

        m_ballPositions = DeviceBuffer<glm::vec4>(GL_SHADER_STORAGE_BUFFER, GL_DYNAMIC_DRAW, num_items);
        m_ballPositions.init();

        m_ballColors = DeviceBuffer<glm::vec4>(GL_SHADER_STORAGE_BUFFER, GL_DYNAMIC_DRAW, num_items);
        m_ballColors.init();
        resetBalls();

        m_vaoRender.init({
            VertexArray::VertexAttribPointer(m_ballVertices.getBufferId(), 4, GL_FLOAT, sizeof(float), GL_FALSE, 0, (void*)0, 0),
            VertexArray::VertexAttribPointer(m_ballPositions.getBufferId(), 4, GL_FLOAT, sizeof(float), GL_FALSE, 0, (void*)0, 1),
            VertexArray::VertexAttribPointer(m_ballColors.getBufferId(), 4, GL_FLOAT, sizeof(float), GL_FALSE, 0, (void*)0, 1)
        });


        m_resetLLProgram.setup("int");
        m_resetLLToProcessProgram.setup("int");
        m_interactionsProgram.setup(width, height);
    }

    void Balls::createBallVertices(float radius)
    {
        m_ballVertices.buffer[0] = glm::vec4(0,0,0,1);
        const auto num = m_ballVertices.buffer.size();
        for(int i=1; i<num; ++i)
        {
            float angle = (i-1) * 2 * PI / (num-2);
            float x = cos(angle) * radius;
            float y = sin(angle) * radius;
            m_ballVertices.buffer[i] = glm::vec4(x,y,0,1);
        }
    }

    void Balls::update(float dt)
    {
        computeBalls(dt);

        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        computeInteractions(dt);


        checkGLError();

        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }

    void Balls::draw(const glm::mat4& position_transform)
    {
        auto transform = position_transform * glm::translate(glm::mat4(1),glm::vec3(-params.width/2,-params.height/2,0));
        m_renderProgram.use();
        m_renderProgram.position_transform.set(transform);
        glEnable(GL_PROGRAM_POINT_SIZE);
        m_vaoRender.bind();
        glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, m_ballVertices.size(), m_ballsProgram.num_items.get());  checkGLError();
        // glDrawArraysInstanced(GL_POINTS, 0, m_ballVertices.size(), m_ballsProgram.num_items.get());  checkGLError();
        m_vaoRender.unbind();
    }

    void Balls::computeBalls(float dt)
    {
        m_resetLLProgram.use();
        m_ll_head.bind().bufferBase(0);
        m_resetLLProgram.dispatch(params.width * params.height, -1);

        m_resetLLProgram.use();
        m_ll_count.bind().bufferBase(0);
        m_resetLLProgram.dispatch(params.width * params.height, 0);

        m_resetLLProgram.use();
        m_ll_next.bind().bufferBase(0);
        m_resetLLProgram.dispatch(m_balls.read().buffer.size(), -1);

        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        m_ballsProgram.use(); checkGLError();

        m_balls.read().bind().bufferBase(0);
        m_balls.write().bind().bufferBase(1);
        m_ballPositions.bind().bufferBase(2);
        m_ballColors.bind().bufferBase(3);
        m_ll_head.bind().bufferBase(4);
        m_ll_next.bind().bufferBase(5);
        m_ll_count.bind().bufferBase(6);

        m_ballsProgram.dispatch(m_balls.read().buffer.size(), dt);
        m_balls.toggle();

        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        // m_ll_count.bind();
        // m_ll_count.download();
    }

    void Balls::computeInteractions(float dt)
    {
        auto num_items = m_balls.read().buffer.size();
        m_resetLLToProcessProgram.use();
        m_toprocess.write().bind().bufferBase(0);
        m_resetLLToProcessProgram.dispatch(num_items, 0, 1);

        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        m_toprocess.toggle();

        m_interactionsProgram.use();
        m_balls.read().bind().bufferBase(0); 
        m_ll_head.bind().bufferBase(1); // m_ll_head.download();
        m_ll_next.bind().bufferBase(2); // m_ll_next.download();
        m_toprocess.read().bind().bufferBase(3); // m_toprocess.read().download();
        m_toprocess.write().bind().bufferBase(4); 
        m_counters.bind().bufferBase(5); 
        m_counters.buffer[0] = 0;
        m_counters.buffer[1] = 0;
        stats.num_interactions = 0;
        m_counters.upload();
        m_neighbor_iterators.bind().bufferBase(6);
        
        glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);
        
        checkGLError();

        int iteration_counter = 1;
        int num_remaining = num_items;
        m_interactionsProgram.dispatch(num_remaining, dt, true);

        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        // m_balls.read().bind();
        // m_balls.read().download();

        // m_toprocess.write().bind();
        // m_toprocess.write().download();

        // m_neighbor_iterators.bind();
        // m_neighbor_iterators.download();


        glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);

        m_toprocess.toggle();

        m_counters.bind();
        m_counters.download();
        glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);
        num_remaining = m_counters.buffer[0];
        stats.num_interactions += m_counters.buffer[1];
        assert(sizeof(glm::ivec3) == sizeof(int32_t) * 3);
        while ((iteration_counter < params.max_interaction_iterations) && (num_remaining > 0))
        {
            m_toprocess.read().bind().bufferBase(3);
            m_toprocess.write().bind().bufferBase(4);
            m_counters.bind();
            m_counters.buffer[0] = 0;
            m_counters.buffer[1] = 0;
            m_counters.upload();
            
            glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);

            m_interactionsProgram.dispatch(num_remaining, dt, false);
            
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

            // m_balls.read().bind();
            // m_balls.read().download();

            // m_toprocess.write().bind();
            // m_toprocess.write().download();

            // m_neighbor_iterators.bind();
            // m_neighbor_iterators.download();

            glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);

            m_toprocess.toggle();

            m_counters.bind();
            m_counters.download();
            glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);
            num_remaining = m_counters.buffer[0];
            stats.num_interactions += m_counters.buffer[1];
            ++iteration_counter;
        }
    }

    void Balls::clear()
    {
        resetBalls();
    }


    void Balls::resetBalls()
    {
        resetBalls(0, m_balls[0].buffer.size());
        m_balls.setRead(0);
    }
    void Balls::resizeBalls(int new_size)
    {
        auto old_size = m_balls.read().buffer.size();
        if (new_size == old_size) return;
        if (new_size < 0) new_size = 0;

        m_balls.read().bind();
        m_balls.read().download(0, new_size);
        m_balls.read().buffer.resize(new_size);
        m_balls.write().buffer = m_balls.read().buffer;
        m_ballPositions.resize(new_size);
        m_ballColors.resize(new_size);

        m_ll_next.resize(new_size);
        m_toprocess.read().resize(new_size);
        m_toprocess.write().resize(new_size);
        m_neighbor_iterators.resize(new_size);

        // m_balls.read().buffer.resize(new_size);
        // m_balls.write().buffer.resize(new_size);

        if (new_size > old_size)
        {
            resetBalls(old_size, new_size, false);
        }
        m_balls.read().upload();
        m_balls.write().upload();
    }
    void Balls::resetBalls(int start, int end, bool do_upload)
    {
        for (int i = start; i < end; ++i)
        {
            m_balls[0].buffer[i] = Ball();
            // m_balls[0].buffer[i].pos_vel.x = m_random->uniform(0.0f, params.width - 1);
            // m_balls[0].buffer[i].pos_vel.y = m_random->uniform(0.0f, params.height - 1);
            m_balls[0].buffer[i].pos.x = params.width*0.5f + m_random->uniform(-params.creation_box_size/2, params.creation_box_size/2);
            m_balls[0].buffer[i].pos.y = params.height*0.5f + m_random->uniform(-params.creation_box_size/2, params.creation_box_size/2);
            m_balls[0].buffer[i].vel.x = 0*m_random->normal(0.0f, +1.0f);
            m_balls[0].buffer[i].vel.y = 0*m_random->normal(0.0f, +1.0f);
            m_balls[0].buffer[i].radius = params.ball_radius;

            m_balls[1].buffer[i] = m_balls[0].buffer[i];
        }
        if (do_upload)
        {
            m_balls[0].bind(); m_balls[0].upload(start, end-start); checkGLError();
            m_balls[1].bind(); m_balls[1].upload(start, end-start); checkGLError();
        }

    }

} // namespace balls

// notes:
// 
// find interactions between items in cell and items in cell and its neighbor cells
// for (int i=0; i<count[x,y]; ++i)
// for (int k=i+1; k<count[x,y]; ++k)
// may_interact(cells[x,y][i], cells[x,y][k]);
// 
// for (int dy=-1; dy<=+1; dy++)
// for (int dx=-1; dx<=+1; dx++)
// {
//     if ((dy == 0) && (dx == 0)) continue;
//     for (int i=0; i<count[x,y]; ++i)
//     for (int k=0; k<count[x+dx,y+dy]; ++k)
//     may_interact(cells[x,y][i], cells[x+dx,y+dy][k]);
// }
// num calls:
// (count[x,y]*count[x,y] - count[x,y])/2
// count[x,y]*sum(count[x+dx,y+dy], dx=-1..+1, dy=-1..+1)
// 
// statt linked list zu bauen build sequence with multi-pass
// pass-1 over balls: count items in cells
// pass-2 over cells: atomically get offset of cell storage using the known item counts per cell 
// pass-3 over balls: atomically write ball indices into cell storages (total storage requirement is linear to number of balls)
// 
// 
// 
// 
// for each linked list: 
// atomically enough allocate space to fit its items
// write linked lists items into the allocated space
// 
