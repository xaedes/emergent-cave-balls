#pragma once

#include <string>
#include <vector>
#include <utility>
#include "gl_classes/imgui_gl.h"
#include "gl_classes/program.h"
#include "gui_application/shader/default_shaders.h"
#include "gl_classes/program_uniform.h"

namespace gui_application {
namespace shader {

    struct DefaultProgram : public gl_classes::Program
    {
        template <class T> using ProgramUniform = gl_classes::ProgramUniform<T>;
        DefaultProgram()
            : Program("Default", {defaultVertexShader(), defaultFragmentShader()})
        {}
        void setup() override
        {
            m_shaders[0].setup();
            m_shaders[1].setup();
            gl_classes::Program::setup();
            projection_view_model.init(getGlProgram(), "projectionViewModel");
            instanced.init(getGlProgram(), "instanced");
            pointSize.init(getGlProgram(), "pointSize");
        }
        ProgramUniform<glm::mat4> projection_view_model;
        ProgramUniform<int> instanced;
        ProgramUniform<float> pointSize;
    };


} // namespace shader
} // namespace gui_application
