#pragma once

#include <string>

#include "glm/glm.hpp"

#include "gl_classes/program.h"
#include "gl_classes/program_uniform.h"
#include "gl_classes/program.h"
#include "gl_classes/shader.h"

#include "im_param/im_param.h"
#include "im_param/gl_classes/program_uniform_specializations.h"

namespace balls {

    class RenderProgram : public gl_classes::Program
    {
    public:
        using Program = gl_classes::Program;
        using Shader = gl_classes::Shader;
        template<class T> using ProgramUniform = gl_classes::ProgramUniform<T>;

        RenderProgram();
        ~RenderProgram(){}

        void setup();

        static const std::string code_vertex;
        static const std::string code_fragment;

        ProgramUniform<float> point_size;
        ProgramUniform<glm::mat4> position_transform;
    };
}  // namespace balls


namespace im_param {

    template <class backend_type>
    backend_type& parameter(
        backend_type& backend,
        ::balls::RenderProgram& program, 
        const TypeHolder<::balls::RenderProgram>&)
    {
        parameter(backend, "point_size", program.point_size, 0, 128);

        return backend;
    }
    
} // namespace im_param
