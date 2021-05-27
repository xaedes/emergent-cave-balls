#pragma once

#include <array>
#include <vector>
#include <iostream>
#include <algorithm>
#include <glm/glm.hpp>
// #include <glm/gtc/type_aligned.hpp>
#include "gui_application/drawing/drawable.h"
#include "gl_classes/device_buffer.h"
#include "gl_classes/host_device_buffer.h"
#include "gl_classes/vertex_array.h"

namespace gui_application {
namespace drawing {

    class CubeInstanced : public Drawable
    {
    public:
        using pointer = CubeInstanced*;

        class CubeInstance : public Drawable
        {
        public:
            using Drawable::Drawable;
            using pointer = CubeInstance*;
            virtual ~CubeInstance() {}
        };

        template <typename... Args>
        CubeInstance::pointer Cube(Drawable::memory_pool_type& pool, Args&& ... args)
        {
            return Drawable::Instance<CubeInstanced::CubeInstance>(pool, this, std::forward< Args >(args)...);
        }

        using Drawable::Drawable;
        virtual ~CubeInstanced() {}

        void setup() override
        {
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            glFrontFace(GL_CCW);

            m_vertexBuffer = gl_classes::DeviceBuffer<glm::vec3>(GL_ARRAY_BUFFER, GL_STATIC_DRAW);
            m_vertexBuffer.init();
            m_vertexBuffer.upload(m_vertexBufferData.data(), 0, m_vertexBufferData.size());

            m_colorBuffer = gl_classes::DeviceBuffer<glm::vec3>(GL_ARRAY_BUFFER, GL_STATIC_DRAW);
            m_colorBuffer.init();
            m_colorBuffer.upload(m_colorBufferData.data(), 0, m_colorBufferData.size());

            m_pvmBuffer = gl_classes::HostDeviceBuffer<glm::mat4>(GL_ARRAY_BUFFER, GL_STATIC_DRAW);
            m_pvmBuffer.init();
            // m_pvmBuffer.buffer.push_back(glm::mat4(1));
            m_pvmBuffer.upload();
            m_pvmChanged = true;

            checkGLError();
            m_vertexArray.init({
               gl_classes::VertexArray::VertexAttribPointer(m_vertexBuffer.getBufferId(), 3, GL_FLOAT, sizeof(float), GL_FALSE, 0, (void*)0, 0),
               gl_classes::VertexArray::VertexAttribPointer(m_colorBuffer.getBufferId(), 3, GL_FLOAT, sizeof(float), GL_FALSE, 0, (void*)0, 0),
               gl_classes::VertexArray::VertexAttribPointer(m_pvmBuffer.getBufferId(), 16, GL_FLOAT, sizeof(float), GL_FALSE, 0, (void*)0, 1)
            });
            checkGLError();

        }

        void draw() override
        {
            //sortInstances();
            uploadPvm();
            if (m_pvmBuffer.size() == 0) return;
            bind();
            glDrawArraysInstanced(GL_TRIANGLES, 0, 12 * 3, m_pvmBuffer.size());  checkGLError();
            unbind();
            m_pvmBuffer.buffer.clear();
            m_pvmChanged = true;
        }

        void drawInstance(const glm::mat4& pvm) override
        {
            m_pvmBuffer.buffer.push_back(pvm);
            m_pvmChanged = true;
        }
        bool isInstancedRenderer() const override { return true; }


    protected:
        void bind()
        {
             // glBindVertexArray(m_vertexArray); checkGLError();
            m_vertexArray.bind(); checkGLError();
        }

        void sortInstances()
        {
            if (!m_pvmChanged) return;
            // vertex shader computes gl_Position = pvm * vec4(vertex, 1)
            // we sort by viewing direction of gl_Position with vertex == vec3(0,0,0), which is just pvm[3] (col major)
            // because of projective part, we must sort by pvm[3][2] divided by pvm[3][3]
            std::sort(m_pvmBuffer.buffer.begin(), m_pvmBuffer.buffer.end(), [](const auto& a, const auto& b) {
                return (
                    ((a[3][3] == 0) || (b[3][3] == 0))
                    ? false
                    : ((a[3][2] / a[3][3]) > (b[3][2] / b[3][3]))
                );
            });
        }

        void uploadPvm()
        {
            if (!m_pvmChanged) return;
            m_pvmBuffer.bind(); checkGLError();
            m_pvmBuffer.upload(); checkGLError();
            m_pvmChanged = false; 
        }

        void unbind()
        {
            m_vertexArray.unbind();
 
        }

        // GLuint m_vertexArray;
        gl_classes::VertexArray m_vertexArray;
        gl_classes::DeviceBuffer<glm::vec3> m_vertexBuffer;
        gl_classes::DeviceBuffer<glm::vec3> m_colorBuffer;
        gl_classes::HostDeviceBuffer<glm::mat4> m_pvmBuffer;
        bool m_pvmChanged;



        // http://www.opengl-tutorial.org/beginners-tutorials/tutorial-4-a-colored-cube/
        // Our vertices. Three consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
        // A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices
        static constexpr std::array<glm::vec3,12*3> m_vertexBufferData = {
            glm::vec3(-1.0f, -1.0f, -1.0f), // triangle 1 : begin
            glm::vec3(-1.0f, -1.0f, +1.0f),
            glm::vec3(-1.0f, +1.0f, +1.0f), // triangle 1 : end
            glm::vec3(+1.0f, +1.0f, -1.0f), // triangle 2 : begin
            glm::vec3(-1.0f, -1.0f, -1.0f),
            glm::vec3(-1.0f, +1.0f, -1.0f), // triangle 2 : end
            glm::vec3(+1.0f, -1.0f, +1.0f),
            glm::vec3(-1.0f, -1.0f, -1.0f),
            glm::vec3(+1.0f, -1.0f, -1.0f),
            glm::vec3(+1.0f, +1.0f, -1.0f),
            glm::vec3(+1.0f, -1.0f, -1.0f),
            glm::vec3(-1.0f, -1.0f, -1.0f),
            glm::vec3(-1.0f, -1.0f, -1.0f),
            glm::vec3(-1.0f, +1.0f, +1.0f),
            glm::vec3(-1.0f, +1.0f, -1.0f),
            glm::vec3(+1.0f, -1.0f, +1.0f),
            glm::vec3(-1.0f, -1.0f, +1.0f),
            glm::vec3(-1.0f, -1.0f, -1.0f),
            glm::vec3(-1.0f, +1.0f, +1.0f),
            glm::vec3(-1.0f, -1.0f, +1.0f),
            glm::vec3(+1.0f, -1.0f, +1.0f),
            glm::vec3(+1.0f, +1.0f, +1.0f),
            glm::vec3(+1.0f, -1.0f, -1.0f),
            glm::vec3(+1.0f, +1.0f, -1.0f),
            glm::vec3(+1.0f, -1.0f, -1.0f),
            glm::vec3(+1.0f, +1.0f, +1.0f),
            glm::vec3(+1.0f, -1.0f, +1.0f),
            glm::vec3(+1.0f, +1.0f, +1.0f),
            glm::vec3(+1.0f, +1.0f, -1.0f),
            glm::vec3(-1.0f, +1.0f, -1.0f),
            glm::vec3(+1.0f, +1.0f, +1.0f),
            glm::vec3(-1.0f, +1.0f, -1.0f),
            glm::vec3(-1.0f, +1.0f, +1.0f),
            glm::vec3(+1.0f, +1.0f, +1.0f),
            glm::vec3(-1.0f, +1.0f, +1.0f),
            glm::vec3(+1.0f, -1.0f, +1.0f)
        };

        static constexpr std::array<glm::vec3,12*3> m_colorBufferData = {
            glm::vec3( 0.0f,  0.0f,  0.0f), // triangle 1 : begin
            glm::vec3( 0.0f,  0.0f, +1.0f),
            glm::vec3( 0.0f, +1.0f, +1.0f), // triangle 1 : end
            glm::vec3(+1.0f, +1.0f,  0.0f), // triangle 2 : begin
            glm::vec3( 0.0f,  0.0f,  0.0f),
            glm::vec3( 0.0f, +1.0f,  0.0f), // triangle 2 : end
            glm::vec3(+1.0f,  0.0f, +1.0f),
            glm::vec3( 0.0f,  0.0f,  0.0f),
            glm::vec3(+1.0f,  0.0f,  0.0f),
            glm::vec3(+1.0f, +1.0f,  0.0f),
            glm::vec3(+1.0f,  0.0f,  0.0f),
            glm::vec3( 0.0f,  0.0f,  0.0f),
            glm::vec3( 0.0f,  0.0f,  0.0f),
            glm::vec3( 0.0f, +1.0f, +1.0f),
            glm::vec3( 0.0f, +1.0f,  0.0f),
            glm::vec3(+1.0f,  0.0f, +1.0f),
            glm::vec3( 0.0f,  0.0f, +1.0f),
            glm::vec3( 0.0f,  0.0f,  0.0f),
            glm::vec3( 0.0f, +1.0f, +1.0f),
            glm::vec3( 0.0f,  0.0f, +1.0f),
            glm::vec3(+1.0f,  0.0f, +1.0f),
            glm::vec3(+1.0f, +1.0f, +1.0f),
            glm::vec3(+1.0f,  0.0f,  0.0f),
            glm::vec3(+1.0f, +1.0f,  0.0f),
            glm::vec3(+1.0f,  0.0f,  0.0f),
            glm::vec3(+1.0f, +1.0f, +1.0f),
            glm::vec3(+1.0f,  0.0f, +1.0f),
            glm::vec3(+1.0f, +1.0f, +1.0f),
            glm::vec3(+1.0f, +1.0f,  0.0f),
            glm::vec3( 0.0f, +1.0f,  0.0f),
            glm::vec3(+1.0f, +1.0f, +1.0f),
            glm::vec3( 0.0f, +1.0f,  0.0f),
            glm::vec3( 0.0f, +1.0f, +1.0f),
            glm::vec3(+1.0f, +1.0f, +1.0f),
            glm::vec3( 0.0f, +1.0f, +1.0f),
            glm::vec3(+1.0f,  0.0f, +1.0f)
        };
    };

} // namespace drawing
} // namespace gui_application
