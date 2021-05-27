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

#include "gui_application/random.h"

#include "geometry/pi.h"

#include "gl_classes/compute_programs/set_sequence_program.h"
#include "gl_classes/compute_programs/set_values_program.h"

#include "ants/ant.h"
#include "ants/ants_program.h"
#include "ants/ants_build_grid_linked_list_program.h"
#include "ants/ants_apply_environment_output_program.h"
#include "ants/convolve_program.h"

#include "im_param/im_param.h"

namespace ants {

    struct Ants
    {
        template<class T> using HostDeviceBuffer = gl_classes::HostDeviceBuffer<T>;
        template<class T> using PingPong = gl_classes::PingPong<T>;
        template<class T> using ProgramUniform = gl_classes::ProgramUniform<T>;
        template<class T> using SetSequenceProgram = gl_classes::compute_programs::SetSequenceProgram<T>;
        template<class T> using SetValuesProgram = gl_classes::compute_programs::SetValuesProgram<T>;
        using ComputeProgram = gl_classes::ComputeProgram;
        using Program = gl_classes::Program;
        using Shader = gl_classes::Shader;

        struct Parameters
        {
            struct WorldGenerator
            {
                float food_interval_x = 128;
                float food_interval_y = 128;
                float food_offset_x = 64;
                float food_offset_y = 64;
                float food_radius = 32;
            };
            WorldGenerator world_generator;
            bool vsync = true;
            uint32_t max_ll_traversal_iterations = 16;
            int width = 1024;
            int height = 1024;
        };

        Parameters params;
        void parameterUpdate();

        void setup(gui_application::Random& random);
        void update(double dt);
        void param_gui();

        GLuint makeTexture2D();

        void computeAnts(double dt);
        void computeBlurEnv(double dt);
        void computeBlurEnvSeperated(double dt);
        void computeBlurDisplay(double dt);

        void computeAntsLL();

        void clear();
        void clearEnvironment();
        void clearDisplay();
        void resetAnts();
        void resizeAnts(int new_size);
        void resetAnts(int start, int end, bool do_upload=true);

    public:

        PingPong<HostDeviceBuffer<Ant>> m_ants;
        HostDeviceBuffer<int32_t> m_ants_ll_head;
        HostDeviceBuffer<int32_t> m_ants_ll_next;
        PingPong<HostDeviceBuffer<int32_t>> m_ants_ll_toprocess;
        HostDeviceBuffer<int32_t> m_ants_ll_num_remaining;
        // HostDeviceBuffer<glm::vec2> m_texCoordBuffer;
        // HostDeviceBuffer<glm::u8vec4> m_texDataBuffer;
        

        std::vector<glm::vec4> m_textureDataBuffer;
        std::vector<glm::vec4> m_bufKernelBlurEnv;
        std::vector<glm::vec4> m_bufKernelBlurEnvLine;
        std::vector<glm::vec4> m_bufKernelBlurDisplay;
        
        // Program m_texturedRenderProgram;
        PingPong<GLuint> m_texIdsEnv;
        PingPong<GLuint> m_texIdsDisplay;
        GLuint m_texIdKernelBlurEnv;
        GLuint m_texIdKernelBlurEnvLine;
        GLuint m_texIdKernelBlurDisplay;
        // std::array<GLuint,2> m_texIds;



        AntsProgram m_antsProgram;
        ConvolveProgram m_blurEnvProgram;
        ConvolveProgram m_blurEnvLineProgram;
        ConvolveProgram m_blurDisplayProgram;

        SetValuesProgram<int32_t>         m_antsResetLLProgram;
        AntsBuildGridLinkedListProgram    m_antsBuildLLProgram;
        SetSequenceProgram<int32_t>       m_antsResetLLRemainingProgram;
        AntsApplyEnvironmentOutputProgram m_antsProcessLLProgram;

    protected:
        gui_application::Random* m_random;
    };



} // namespace ants

namespace im_param {


    template <class backend_type>
    backend_type& parameter(
        backend_type& backend,
        ::ants::Ants& ants, 
        const TypeHolder<::ants::Ants>&)
    {
        parameter(backend, "Parameters", ants.params, TypeHolder<ants::Ants>());
        parameter(backend, "AntsProgram", ants.m_antsProgram, TypeHolder<ants::AntsProgram>());
        parameter(backend, "Environment Blur Params", ants.m_blurEnvProgram, TypeHolder<ants::ConvolveProgram>());
        parameter(backend, "Ants Display Blur Params", ants.m_blurDisplayProgram, TypeHolder<ants::ConvolveProgram>());
        parameter(backend, "max_ll_traversal", ants.m_antsProcessLLProgram.max_traverse, 0, 255);
        parameter(backend, "max_ll_traversal_iterations", ants.params.max_ll_traversal_iterations, 0, 2048);
        return backend;
    }

    template <class backend_type>
    backend_type& parameter(
        backend_type& backend,
        ::ants::Ants::Parameters& params, 
        const TypeHolder<::ants::Ants>& th)
    {
        parameter(backend, "world_generator", params.world_generator, th);
        parameter(backend, "vsync", params.vsync);
        // parameter(backend, "width", params.width, 1, 2048);
        // parameter(backend, "height", params.height, 1, 2048);
        return backend;
    }

    template <class backend_type>
    backend_type& parameter(
        backend_type& backend,
        ::ants::Ants::Parameters::WorldGenerator& params, 
        const TypeHolder<::ants::Ants>&)
    {
        parameter(backend, "food_interval_x", params.food_interval_x, 1, 255);
        parameter(backend, "food_interval_y", params.food_interval_y, 1, 255);
        parameter(backend, "food_offset_x",   params.food_offset_x, -255, 255);
        parameter(backend, "food_offset_y",   params.food_offset_y, -255, 255);
        parameter(backend, "food_radius",     params.food_radius, 0, 255);
        return backend;
    }
    
} // namespace im_param

