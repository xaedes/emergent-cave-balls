#pragma once

#include <cstdint>
#include <vector>

#include "glm/glm.hpp"


#include "gl_classes/host_device_buffer.h"
#include "gl_classes/program.h"
#include "gl_classes/program_uniform.h"
#include "gl_classes/compute_program.h"
#include "gl_classes/shader.h"
#include "gl_classes/ping_pong.h"
#include "gl_classes/vertex_array.h"

#include "gui_application/random.h"

#include "geometry/pi.h"

#include "gl_classes/compute_programs/set_sequence_program.h"
#include "gl_classes/compute_programs/set_values_program.h"

#include "balls/ball.h"
#include "balls/balls_program.h"
#include "balls/interactions_program.h"
#include "balls/render_program.h"


#include "im_param/im_param.h"

namespace balls {

    struct Balls
    {
        template<class T> using DeviceBuffer = gl_classes::DeviceBuffer<T>;
        template<class T> using HostDeviceBuffer = gl_classes::HostDeviceBuffer<T>;
        template<class T> using PingPong = gl_classes::PingPong<T>;
        template<class T> using ProgramUniform = gl_classes::ProgramUniform<T>;
        template<class T> using SetSequenceProgram = gl_classes::compute_programs::SetSequenceProgram<T>;
        template<class T> using SetValuesProgram = gl_classes::compute_programs::SetValuesProgram<T>;
        using ComputeProgram = gl_classes::ComputeProgram;
        using Program = gl_classes::Program;
        using Shader = gl_classes::Shader;
        using VertexArray = gl_classes::VertexArray;

        struct Parameters
        {
            bool vsync = true;

            int32_t width = 512;
            int32_t height = 512;

            float creation_box_size = 2;

            float ball_radius = 0.5;
            int32_t max_interaction_iterations = 10240;
        };

        struct Stats
        {
            int num_interactions = 0;
        };

        Parameters params;
        Stats stats;
        void parameterUpdate();

        void setup(gui_application::Random& random);
        void update(float dt);
        void draw(const glm::mat4& position_transform);

        void computeBalls(float dt);
        void computeInteractions(float dt);

        void clear();
        void resetBalls();
        void resizeBalls(int new_size);
        void resetBalls(int start, int end, bool do_upload=true);

        void createBallVertices(float radius=0.5f);

    public:

        PingPong<HostDeviceBuffer<Ball>> m_balls;       // shape [2x[N]]
        DeviceBuffer<glm::vec4> m_ballPositions;        // shape [N]
        DeviceBuffer<glm::vec4> m_ballColors;           // shape [N]
        HostDeviceBuffer<glm::vec4> m_ballVertices;     // shape [V]

        HostDeviceBuffer<int32_t> m_ll_count;               // shape [W*H]
        HostDeviceBuffer<int32_t> m_ll_head;                // shape [W*H]
        HostDeviceBuffer<int32_t> m_ll_next;                // shape [N] 
        PingPong<HostDeviceBuffer<int32_t>> m_toprocess;    // shape [2x[N]] 
        HostDeviceBuffer<int32_t> m_counters;               // shape [2]
        HostDeviceBuffer<glm::ivec4> m_neighbor_iterators;  // shape [N]

        VertexArray m_vaoRender;

        BallsProgram m_ballsProgram;

        InteractionsProgram m_interactionsProgram;

        SetValuesProgram<int32_t>   m_resetLLProgram;
        SetSequenceProgram<int32_t> m_resetLLToProcessProgram;

        RenderProgram m_renderProgram;

    protected:
        gui_application::Random* m_random;
    };



} // namespace balls

namespace im_param {

    template <class backend_type>
    backend_type& parameter(
        backend_type& backend,
        ::balls::Balls& balls, 
        const TypeHolder<::balls::Balls>&th)
    {
        parameter(backend, "Parameters", balls.params, th);
        parameter(backend, "BallsProgram", balls.m_ballsProgram, TypeHolder<balls::BallsProgram>());
        parameter(backend, "InteractionsProgram", balls.m_interactionsProgram, TypeHolder<balls::InteractionsProgram>());
        parameter(backend, "RenderProgram", balls.m_renderProgram, TypeHolder<balls::RenderProgram>());
        return backend;
    }

    template <class backend_type>
    backend_type& parameter(
        backend_type& backend,
        ::balls::Balls::Parameters& params, 
        const TypeHolder<::balls::Balls>& th)
    {
        parameter(backend, "vsync", params.vsync);
        parameter(backend, "width", params.width, 1, 2048);
        parameter(backend, "height", params.height, 1, 2048);
        parameter(backend, "ball_radius", params.ball_radius, 0, 255);
        parameter(backend, "creation_box_size", params.creation_box_size, 0, 255);
        parameter(backend, "max_interaction_iterations", params.max_interaction_iterations, 0, 1024);
        return backend;
    }

} // namespace im_param
