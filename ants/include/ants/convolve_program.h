#pragma once

#include <string>
#include "glm/glm.hpp"

#include "gl_classes/program.h"
#include "gl_classes/program_uniform.h"
#include "gl_classes/compute_program.h"
#include "gl_classes/shader.h"

#include "im_param/im_param.h"
#include "im_param/gl_classes/program_uniform_specializations.h"

namespace ants {

    class ConvolveProgram : public gl_classes::ComputeProgram
    {
    public:
        using Program = gl_classes::Program;
        using ComputeProgram = gl_classes::ComputeProgram;
        using Shader = gl_classes::Shader;
        template<class T> using ProgramUniform = gl_classes::ProgramUniform<T>;

        ConvolveProgram(){}
        ~ConvolveProgram(){}
        void setup();
        void dispatch(int width, int height, float dt);
        std::string code() const;
        // ProgramUniform<glm::mat4> projection_view_model;
        ProgramUniform<int32_t> dx;
        ProgramUniform<int32_t> dy;
        ProgramUniform<int32_t> width;
        ProgramUniform<int32_t> height;
        ProgramUniform<int32_t> kernel_width;
        ProgramUniform<int32_t> kernel_height;
        ProgramUniform<glm::vec4> multiplier;
        ProgramUniform<glm::vec4> gain;
        ProgramUniform<float> dt;
        ProgramUniform<float> reference_dt;
    };

}  // namespace ants


namespace im_param {

    template <class backend_type>
    backend_type& parameter(
        backend_type& backend,
        ::ants::ConvolveProgram& program, 
        const TypeHolder<::ants::ConvolveProgram>&)
    {
        program.use();
        // parameter(backend, "num_items"    , program.num_items    , 0, 10000);
        // parameter(backend, "width"        , program.width        , 0, 2048);
        // parameter(backend, "height"       , program.height       , 0, 2048);
        
        parameter(backend, "multiplier"   , program.multiplier);
        parameter(backend, "gain"         , program.gain);
        parameter(backend, "reference_dt" , program.reference_dt);
        return backend;
    }
    
} // namespace im_param
