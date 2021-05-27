#pragma once

#include <glm/glm.hpp>
#include "gui_application/drawing/drawable.h"
#include "gl_classes/shader.h"
#include "gl_classes/program.h"
#include "gl_classes/program_uniform.h"
#include "gl_classes/vertex_array.h"
#include "gl_classes/host_device_buffer.h"

namespace gui_application {
namespace drawing {

    class TextureQuad : public Drawable
    {
    public:
        using Drawable::Drawable;
        using pointer = TextureQuad*;

        template <typename... Args>
        TextureQuad(Args&&... args)
           : Drawable(std::forward< Args >( args )...)
           , m_quadSize(1,1)
        {
            customProgram = true;
        }
        template <typename... Args>
        TextureQuad(glm::vec2 size, Args&&... args)
           : Drawable(std::forward< Args >( args )...)
           , m_quadSize(size)
        {
            customProgram = true;
        }
        virtual ~TextureQuad() {}

        void setup() override
        {
            if (!s_program.isValid()) s_program.setup();

            auto hw = m_quadSize.x/2;
            auto hh = m_quadSize.y/2;
            m_vertexBuffer = gl_classes::HostDeviceBuffer<glm::vec3>(GL_ARRAY_BUFFER, GL_STATIC_DRAW, 4, 4);
            m_vertexBuffer.init();
            m_texCoordBuffer = gl_classes::HostDeviceBuffer<glm::vec2>(GL_ARRAY_BUFFER, GL_STATIC_DRAW, 4, 4);
            m_texCoordBuffer.init();
            m_pvmBuffer = gl_classes::HostDeviceBuffer<glm::mat4>(GL_ARRAY_BUFFER, GL_STATIC_DRAW, 1);
            m_pvmBuffer.init();

            m_vertexBuffer.buffer[0] = glm::vec3(-hw,-hh,0);
            m_vertexBuffer.buffer[1] = glm::vec3(-hw,+hh,0);
            m_vertexBuffer.buffer[2] = glm::vec3(+hw,+hh,0);
            m_vertexBuffer.buffer[3] = glm::vec3(+hw,-hh,0);

            m_texCoordBuffer.buffer[0] = glm::vec2(0, 0);
            m_texCoordBuffer.buffer[1] = glm::vec2(0, 1);
            m_texCoordBuffer.buffer[2] = glm::vec2(1, 1);
            m_texCoordBuffer.buffer[3] = glm::vec2(1, 0);

            m_pvmBuffer.buffer[0] = glm::mat4(1);

            m_vertexBuffer.bind();  m_vertexBuffer.upload();
            m_texCoordBuffer.bind(); m_texCoordBuffer.upload();
            m_pvmBuffer.bind(); m_pvmBuffer.upload();

            m_vertexArray.init({
               gl_classes::VertexArray::VertexAttribPointer(m_vertexBuffer.getBufferId(), 3, GL_FLOAT, sizeof(float), GL_FALSE, 0, (void*)0, 0),
               gl_classes::VertexArray::VertexAttribPointer(m_texCoordBuffer.getBufferId(), 2, GL_FLOAT, sizeof(float), GL_FALSE, 0, (void*)0, 0),
               gl_classes::VertexArray::VertexAttribPointer(m_pvmBuffer.getBufferId(), 16, GL_FLOAT, sizeof(float), GL_FALSE, 0, (void*)0, 1)
            });
            checkGLError();

        }
        void draw() override
        {
            // glDrawArrays(GL_TRIANGLES, 0, 3);  checkGLError();
            // glDrawArraysInstanced(GL_LINES, 0, 2, m_pvmBuffer.size());  checkGLError();
            // glDrawArraysInstanced(GL_TRIANGLES, 0, 3, m_pvmBuffer.size());  checkGLError();
            glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, m_pvmBuffer.size());  checkGLError();
        }
        void draw(const glm::mat4& pvm) override
        {
            if (textureId != 0)
            {
                bind();
                s_program.projection_view_model.set(pvm);
                s_program.instanced.set(0);
                draw();
                unbind();
                checkGLError();
            }
        }

        GLuint textureId = 0;

    protected:
        void bind()
        {
            s_program.use();
            glBindTexture(GL_TEXTURE_2D, textureId);
            m_vertexArray.bind(); checkGLError();
        }
        void unbind()
        {
            m_vertexArray.unbind();
        }
        gl_classes::HostDeviceBuffer<glm::vec3> m_vertexBuffer;
        // gl_classes::HostDeviceBuffer<glm::vec3> m_colorBuffer;
        gl_classes::HostDeviceBuffer<glm::vec2> m_texCoordBuffer;
        gl_classes::HostDeviceBuffer<glm::mat4> m_pvmBuffer;
        // gl_classes::HostDeviceBuffer<glm::u8vec4> m_texDataBuffer;
        gl_classes::VertexArray m_vertexArray;

        glm::vec2 m_quadSize;

    public:
        static class Program : public gl_classes::Program
        {
        public:
            static std::string VertexShaderCode() { return 
"                                                                 \n\
#version 330 core                                                 \n\
layout(location = 0) in vec3 vert;                                \n\
layout(location = 1) in vec2 vertTexCoord;                        \n\
layout(location = 2) in mat4 projectionViewModelInstances;        \n\
// location 2,3,4,5 occupied by projectionViewModelInstances      \n\
out vec2 fragTexCoord;                                            \n\
uniform mat4 projectionViewModel;                                 \n\
uniform int instanced = 0;                                        \n\
void main() {                                                     \n\
    // Pass the tex coord straight through to the fragment shader \n\
    fragTexCoord = vertTexCoord;                                  \n\
    mat4 actualProjectionViewModel = (instanced != 0)             \n\
        ? projectionViewModelInstances                            \n\
        : projectionViewModel;                                    \n\
                                                                  \n\
    gl_Position = actualProjectionViewModel * vec4(vert, 1);      \n\
}                                                                 \n\
"; 
            }

            static std::string FragmentShaderCode() { return 
"                                                            \n\
#version 330 core                                            \n\
uniform sampler2D tex; //this is the texture                 \n\
in vec2 fragTexCoord; //this is the texture coord            \n\
out vec4 finalColor; //this is the output color of the pixel \n\
                                                             \n\
void main() {                                                \n\
    finalColor = texture(tex, fragTexCoord);                 \n\
    //finalColor = vec4(0,0,0,1);                            \n\
}                                                            \n\
";
            }
            Program() : gl_classes::Program({
                gl_classes::Shader(gl_classes::Shader::ShaderType::Vertex, VertexShaderCode()),
                gl_classes::Shader(gl_classes::Shader::ShaderType::Fragment, FragmentShaderCode()),
            }) {}
            void setup()
            {
                getShaders()[0].setup();
                getShaders()[1].setup();
                gl_classes::Program::setup();
                projection_view_model.init(getGlProgram(), "projectionViewModel");
                instanced.init(getGlProgram(), "instanced");
            }
            gl_classes::ProgramUniform<glm::mat4> projection_view_model;
            gl_classes::ProgramUniform<int> instanced;
        };
        static Program s_program;
        Program createTexturedRenderProgram() const;
        std::string texturedVertexShaderCode() const;
        std::string texturedFragmentShaderCode() const;        
    };

} // namespace drawing
} // namespace gui_application
