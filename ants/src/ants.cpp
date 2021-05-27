#include "ants/ants.h"

namespace ants {
    
    void Ants::parameterUpdate()
    {
        glfwSwapInterval(params.vsync ? 1 : 0);
        resizeAnts(m_antsProgram.num_items.get());
    }

    void Ants::setup(gui_application::Random& random)
    {
        m_random = &random;
        auto width = params.width;
        auto height = params.height;

        m_antsProgram.setup(width, height);

        checkGLError();

        m_blurEnvProgram.setup();
        checkGLError();

        m_blurEnvProgram.kernel_width.set(3);
        m_blurEnvProgram.kernel_height.set(3);
        m_blurEnvProgram.dx.set(-1);
        m_blurEnvProgram.dy.set(-1);
        m_blurEnvProgram.multiplier.set(glm::vec4(0.93,0.969,1,0));
        m_blurEnvProgram.gain.set(glm::vec4(0.05f,0.06f,0.9f,0.9f));
        m_blurEnvProgram.reference_dt.set(0.1f);

        m_blurEnvLineProgram.setup();
        checkGLError();

        m_blurEnvLineProgram.kernel_width.set(5);
        m_blurEnvLineProgram.kernel_height.set(1);
        m_blurEnvLineProgram.dx.set(-2);
        m_blurEnvLineProgram.dy.set(0);
        m_blurEnvLineProgram.multiplier.set(glm::vec4(0.93,0.969,1,0));
        m_blurEnvLineProgram.gain.set(glm::vec4(0.05f,0.06f,0.9f,0.9f));
        m_blurEnvLineProgram.reference_dt.set(0.1f);

        m_blurDisplayProgram.setup();
        checkGLError();

        m_blurDisplayProgram.kernel_width.set(1);
        m_blurDisplayProgram.kernel_height.set(1);
        m_blurDisplayProgram.dx.set(0);
        m_blurDisplayProgram.dy.set(0);
        m_blurDisplayProgram.multiplier.set(glm::vec4(0.0,0,0,0));
        m_blurDisplayProgram.gain.set(glm::vec4(0.15,0.13,0.55,1));
        m_blurDisplayProgram.reference_dt.set(1);
        
        m_antsResetLLProgram.setup("int");
        
        m_antsBuildLLProgram.setup();
        m_antsBuildLLProgram.width.set(width);
        m_antsBuildLLProgram.height.set(height);
        
        m_antsResetLLRemainingProgram.setup("int");
        
        m_antsProcessLLProgram.setup();
        m_antsProcessLLProgram.width.set(width);
        m_antsProcessLLProgram.height.set(height);
        m_antsProcessLLProgram.max_traverse.set(16);

        params.max_ll_traversal_iterations = 16;
        
        // m_texturedRenderProgram = createTexturedRenderProgram();
        // checkGLError();
        // m_texturedRenderProgram.getShaders()[0].setup();
        // m_texturedRenderProgram.getShaders()[1].setup();
        // m_texturedRenderProgram.setup();
        // checkGLError();
        int num_ants = 100;
        // int num_ants = 1024 * 1024;
        // int num_ants = 1024 * 10;
        // int num_ants = 1024;
        m_antsProgram.num_items.set(num_ants);

        for (int i=0; i<2; ++i)
        {
            m_ants[i] = HostDeviceBuffer<Ant>(GL_SHADER_STORAGE_BUFFER, GL_DYNAMIC_DRAW, num_ants, num_ants);
            m_ants_ll_toprocess[i] = HostDeviceBuffer<int32_t>(GL_SHADER_STORAGE_BUFFER, GL_DYNAMIC_DRAW, (width * height), (width * height));
            
            m_ants[i].init();
            m_ants_ll_toprocess[i].init();
        }
        m_ants_ll_head = HostDeviceBuffer<int32_t>(GL_SHADER_STORAGE_BUFFER, GL_DYNAMIC_DRAW, (width * height), (width * height));
        m_ants_ll_next = HostDeviceBuffer<int32_t>(GL_SHADER_STORAGE_BUFFER, GL_DYNAMIC_DRAW, num_ants, num_ants);
        m_ants_ll_num_remaining = HostDeviceBuffer<int32_t>(GL_SHADER_STORAGE_BUFFER, GL_DYNAMIC_DRAW, 1, 1);
        m_ants_ll_head.init();
        m_ants_ll_next.init();
        m_ants_ll_num_remaining.init();
        checkGLError();
        resetAnts();

        checkGLError();
        glEnable(GL_TEXTURE_2D);
        glActiveTexture(GL_TEXTURE0);
        m_texIdsEnv[0] = makeTexture2D();
        m_texIdsEnv[1] = makeTexture2D();
        clearEnvironment();
        m_texIdsDisplay[0] = makeTexture2D();
        m_texIdsDisplay[1] = makeTexture2D();
        clearDisplay();

        // glActiveTexture(GL_TEXTURE0+2);
        m_texIdKernelBlurEnv = makeTexture2D();
        m_bufKernelBlurEnv = {
            glm::vec4(1,1,0,0), glm::vec4(2,2,0,0), glm::vec4(1,1,0,0),
            glm::vec4(2,2,0,0), glm::vec4(3,3,1,1), glm::vec4(2,2,0,0),
            glm::vec4(1,1,0,0), glm::vec4(2,2,0,0), glm::vec4(1,1,0,0),
        };
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 3, 3, 0, GL_RGBA, GL_FLOAT, m_bufKernelBlurEnv.data() );

        m_texIdKernelBlurEnvLine = makeTexture2D();
        m_bufKernelBlurEnvLine = {
            glm::vec4(1,1,0,0), glm::vec4(2,2,0,0), glm::vec4(3,3,0,0), glm::vec4(2,2,0,0), glm::vec4(1,1,0,0)
        };
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 5, 1, 0, GL_RGBA, GL_FLOAT, m_bufKernelBlurEnvLine.data() );
        
        m_texIdKernelBlurDisplay = makeTexture2D();
        m_bufKernelBlurDisplay = {
            glm::vec4(1,1,1,1)
        };
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 1, 1, 0, GL_RGBA, GL_FLOAT, m_bufKernelBlurDisplay.data() );

    }

    GLuint Ants::makeTexture2D()
    {
        GLuint id;
        glGenBuffers(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        return id;
    }


    void Ants::update(double dt)
    {

        computeBlurEnv(dt);
        // computeBlurEnvSeperated(dt);
        computeBlurDisplay(dt);
        
        m_texIdsEnv.toggle();
        m_texIdsDisplay.toggle();

        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        
        computeAnts(dt);

        m_ants.toggle();

        computeAntsLL(); // modifies textures.read() in-place

        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        checkGLError();

    }

    void Ants::computeAnts(double dt)
    {
        m_antsProgram.use(); checkGLError();

        m_ants.read().bind().bufferBase(0);
        m_ants.write().bind().bufferBase(1);
        // glBindTexture(GL_TEXTURE_2D, m_texIdsEnv[m_readTexIdsEnv]);
        // glBindTexture(GL_TEXTURE_2D, m_texIdsEnv[1-m_readTexIdsEnv]);
        glBindImageTexture(0, m_texIdsEnv.read(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
        // glBindImageTexture(1, m_texIdsEnv.write(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

        glBindImageTexture(1, m_texIdsDisplay.read(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
        // glBindImageTexture(3, m_texIdsDisplay.write(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

        m_antsProgram.dispatch(m_ants.read().buffer.size(), dt);
    }

    void Ants::computeAntsLL()
    {
        m_antsResetLLProgram.use();
        m_ants_ll_head.bind().bufferBase(0);
        m_antsResetLLProgram.dispatch(params.width * params.height, -1); glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);


        m_antsBuildLLProgram.use();
        m_ants.read().bind().bufferBase(0);  //m_ants.read().download();
        m_ants_ll_head.bind().bufferBase(1); //m_ants_ll_head.download();
        m_ants_ll_next.bind().bufferBase(2); //m_ants_ll_next.download();

        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        m_antsBuildLLProgram.dispatch(m_ants.read().buffer.size()); glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        
        //m_ants.read().bind();  m_ants.read().download();
        //m_ants_ll_head.bind(); m_ants_ll_head.download();
        //m_ants_ll_next.bind(); m_ants_ll_next.download();
        
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        m_antsResetLLRemainingProgram.use();
        m_ants_ll_toprocess.read().bind().bufferBase(0);
        m_antsResetLLRemainingProgram.dispatch(params.width * params.height, 0, 1); glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        
        //m_ants_ll_toprocess.read().download();

        m_antsProcessLLProgram.use();
        m_ants.read().bind().bufferBase(0);
        m_ants_ll_head.bind().bufferBase(1);
        m_ants_ll_next.bind().bufferBase(2);
        m_ants_ll_num_remaining.bind().bufferBase(5);


        // works in-place on textures, hence we can just modify the current read
        glBindImageTexture(0, m_texIdsEnv.read(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
        glBindImageTexture(1, m_texIdsDisplay.read(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);

        int num_remaining = params.width * params.height;
        int num_iterations = 0;
        while ((num_remaining > 0) && (num_iterations < params.max_ll_traversal_iterations))
        {
            m_ants_ll_toprocess.read().bind().bufferBase(3);
            m_ants_ll_toprocess.write().bind().bufferBase(4);
            m_ants_ll_num_remaining.bind();
            m_ants_ll_num_remaining.buffer[0] = 0;
            m_ants_ll_num_remaining.upload();

            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
            m_antsProcessLLProgram.dispatch(num_remaining);

            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
            //m_ants_ll_head.bind(); m_ants_ll_head.download();
            //m_ants_ll_toprocess.write().bind();
            //m_ants_ll_toprocess.write().download();
            m_ants_ll_num_remaining.bind();
            m_ants_ll_num_remaining.download();
            num_remaining = m_ants_ll_num_remaining.buffer[0];
            m_ants_ll_toprocess.toggle();
            ++num_iterations;
        }
    }

    void Ants::computeBlurEnv(double dt)
    {
        m_blurEnvProgram.use(); checkGLError();

        glBindTexture(GL_TEXTURE_2D, m_texIdsEnv.read());
        glBindImageTexture(0, m_texIdsEnv.read(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
        glBindTexture(GL_TEXTURE_2D, m_texIdsEnv.write());
        glBindImageTexture(1, m_texIdsEnv.write(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
        glBindTexture(GL_TEXTURE_2D, m_texIdKernelBlurEnv);
        glBindImageTexture(2, m_texIdKernelBlurEnv, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);

        m_blurEnvProgram.dispatch(params.width, params.height, dt);
    }
    void Ants::computeBlurEnvSeperated(double dt)
    {
        m_blurEnvLineProgram.use(); checkGLError();

        glBindTexture(GL_TEXTURE_2D, m_texIdsEnv.read());
        glBindImageTexture(0, m_texIdsEnv.read(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
        glBindTexture(GL_TEXTURE_2D, m_texIdsEnv.write());
        glBindImageTexture(1, m_texIdsEnv.write(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
        glBindTexture(GL_TEXTURE_2D, m_texIdKernelBlurEnv);
        glBindImageTexture(2, m_texIdKernelBlurEnv, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);

        m_blurEnvLineProgram.kernel_width.set(5);
        m_blurEnvLineProgram.kernel_height.set(1);
        m_blurEnvLineProgram.dx.set(-2);
        m_blurEnvLineProgram.dy.set(0);
        m_blurEnvLineProgram.dispatch(params.width, params.height, dt);

        m_texIdsEnv.toggle();
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        glBindTexture(GL_TEXTURE_2D, m_texIdsEnv.read());
        glBindImageTexture(0, m_texIdsEnv.read(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
        glBindTexture(GL_TEXTURE_2D, m_texIdsEnv.write());
        glBindImageTexture(1, m_texIdsEnv.write(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
        glBindTexture(GL_TEXTURE_2D, m_texIdKernelBlurEnvLine);
        glBindImageTexture(2, m_texIdKernelBlurEnvLine, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);

        m_blurEnvLineProgram.kernel_width.set(1);
        m_blurEnvLineProgram.kernel_height.set(5);
        m_blurEnvLineProgram.dx.set(0);
        m_blurEnvLineProgram.dy.set(-2);
        m_blurEnvLineProgram.dispatch(params.width, params.height, dt);
    }

    void Ants::computeBlurDisplay(double dt)
    {
        m_blurDisplayProgram.use(); checkGLError();

        glBindTexture(GL_TEXTURE_2D, m_texIdsDisplay.read());
        glBindImageTexture(0, m_texIdsDisplay.read(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
        glBindTexture(GL_TEXTURE_2D, m_texIdsDisplay.write());
        glBindImageTexture(1, m_texIdsDisplay.write(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
        glBindTexture(GL_TEXTURE_2D, m_texIdKernelBlurDisplay);
        glBindImageTexture(2, m_texIdKernelBlurDisplay, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);

        m_blurDisplayProgram.dispatch(params.width, params.height, dt);
    }

    void Ants::clear()
    {
        resetAnts();
        clearEnvironment();
        clearDisplay();
    }

    void Ants::clearEnvironment()
    {
        m_textureDataBuffer.clear();
        m_textureDataBuffer.resize(params.width*params.height, glm::vec4(0,0,0,0));

        for(int y=0; y<params.height; ++y)
        {
            auto* row = &m_textureDataBuffer[y*params.width];
            for(int x=0; x<params.width; ++x)
            {
                float closest_x = x + params.world_generator.food_offset_x - fmod(x, params.world_generator.food_interval_x);
                float closest_y = y + params.world_generator.food_offset_y - fmod(y, params.world_generator.food_interval_y);
                float dx = x - closest_x;
                float dy = y - closest_y;
                float r2 = dx*dx + dy*dy;
                row[x].r = 0;
                row[x].g = 0;
                row[x].b = (r2 < params.world_generator.food_radius*params.world_generator.food_radius) ? 1 : 0;
                row[x].a = 1;
                // row[x] = glm::vec4(0,0,1,1);
            }
        }    
        glBindTexture(GL_TEXTURE_2D, m_texIdsEnv[0]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, params.width, params.height, 0, GL_RGBA, GL_FLOAT, m_textureDataBuffer.data() );
        glBindTexture(GL_TEXTURE_2D, m_texIdsEnv[1]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, params.width, params.height, 0, GL_RGBA, GL_FLOAT, m_textureDataBuffer.data() );

        m_texIdsEnv.setRead(0);
    }
    void Ants::clearDisplay()
    {
        m_textureDataBuffer.clear();
        m_textureDataBuffer.resize(params.width*params.height, glm::vec4(0,0,0,0));
        for(int y=0; y<params.height; ++y)
        {
            auto* row = &m_textureDataBuffer[y*params.width];
            for(int x=0; x<params.width; ++x)
            {
                row[x] = glm::vec4(0,0,0,1);;
            }
        }    
        glBindTexture(GL_TEXTURE_2D, m_texIdsDisplay[0]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, params.width, params.height, 0, GL_RGBA, GL_FLOAT, m_textureDataBuffer.data() );
        glBindTexture(GL_TEXTURE_2D, m_texIdsDisplay[1]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, params.width, params.height, 0, GL_RGBA, GL_FLOAT, m_textureDataBuffer.data() );

        m_texIdsDisplay.setRead(0);
    }
    void Ants::resetAnts()
    {
        resetAnts(0, m_ants[0].buffer.size());
        m_ants.setRead(0);
    }
    void Ants::resizeAnts(int new_size)
    {
        auto old_size = m_ants.read().buffer.size();
        if (new_size == old_size) return;
        if (new_size < 0) new_size = 0;

        m_ants_ll_next.buffer.resize(new_size, -1);
        m_ants_ll_next.bind(); 
        m_ants_ll_next.upload();

        m_ants.read().bind();
        m_ants.read().download(0, new_size);
        m_ants.read().buffer.resize(new_size);
        m_ants.write().buffer = m_ants.read().buffer;

        // m_ants.read().buffer.resize(new_size);
        // m_ants.write().buffer.resize(new_size);

        if (new_size > old_size)
        {
            resetAnts(old_size, new_size, false);
        }
        m_ants.read().upload();
        m_ants.write().upload();
    }
    void Ants::resetAnts(int start, int end, bool do_upload)
    {
        for (int i = start; i < end; ++i)
        {
            m_ants[0].buffer[i] = Ant();
            // m_ants[0].buffer[i].pos_vel.x = m_random->uniform(0.0f, params.width - 1);
            // m_ants[0].buffer[i].pos_vel.y = m_random->uniform(0.0f, params.height - 1);
            m_ants[0].buffer[i].pos_vel.x = params.width/2 + m_random->uniform(-10.0f, 10.0f);
            m_ants[0].buffer[i].pos_vel.y = params.height/2 + m_random->uniform(-10.0f, 10.0f);
            m_ants[0].buffer[i].pos_vel.z = m_random->uniform(0.0f, 2*PI);
            m_ants[0].buffer[i].pos_vel.w = 2; //abs(m_random->normal(0.0f, 1.0f));
            m_ants[0].buffer[i].random.x = i;
            m_ants[0].buffer[i].random.y = i+1*m_ants[0].buffer.size();
            m_ants[0].buffer[i].random.z = i+2*m_ants[0].buffer.size();
            m_ants[0].buffer[i].random.w = i+3*m_ants[0].buffer.size();
            m_ants[0].buffer[i].maxage_age_grip.x = m_random->uniform(m_antsProgram.min_maxage.get(), m_antsProgram.max_maxage.get());
            m_ants[1].buffer[i] = m_ants[0].buffer[i];
        }
        if (do_upload)
        {
            m_ants[0].bind(); m_ants[0].upload(start, end-start); checkGLError();
            m_ants[1].bind(); m_ants[1].upload(start, end-start); checkGLError();
        }

    }

} // namespace ants
