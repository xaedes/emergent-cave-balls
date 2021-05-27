
#include "balls/render_program.h"
#include "balls/ball.h"

#include "geometry/pi.h"

#include "im_param/backends/glsl_struct_generator_backend.h"

namespace balls {

    RenderProgram::RenderProgram()
        : gl_classes::Program({
            Shader(Shader::ShaderType::Vertex, code_vertex),
            Shader(Shader::ShaderType::Fragment, code_fragment)
        })
    {}

    void RenderProgram::setup()
    {
        Ball ball;
        im_param::GlslStructGeneratorBackend glslStructGenerator;
        im_param::parameter(glslStructGenerator, "Ball", ball, im_param::TypeHolder<Ball>());

        m_shaders[0].setup();
        m_shaders[1].setup();

        gl_classes::Program::setup();
        point_size.init(getGlProgram(), "point_size", 16);
        position_transform.init(getGlProgram(), "position_transform", glm::mat4(1));
        checkGLError();
    }

    const std::string RenderProgram::code_vertex = (
R"(
#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec4 vertex;
layout(location = 1) in vec4 position;
layout(location = 2) in vec4 color;

out vec3 fragmentColor;
uniform float point_size;
uniform mat4 position_transform;

void main(){
    vec4 scale = vec4(position.z*2,position.z*2,1,0);
    gl_Position =  position_transform * ((scale * vertex) + vec4(position.xy, 0, 1));
    fragmentColor = color.xyz;
    gl_PointSize = point_size;
}
)"
    );  

    const std::string RenderProgram::code_fragment = (
R"(
#version 330 core

// Interpolated values from the vertex shaders
in vec3 fragmentColor;

// Ouput data
out vec3 color;

void main(){
    color = fragmentColor;
}
)"
    );

} // namespace balls
