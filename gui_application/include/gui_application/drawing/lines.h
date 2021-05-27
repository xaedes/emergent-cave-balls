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
#include "gl_classes/non_shrinking_vector.h"
#include "gl_classes/vertex_array.h"

namespace gui_application {
namespace drawing {

    class Lines : public Drawable
    {
    public:
        using Drawable::Drawable;
        using pointer = Lines*;
        virtual ~Lines() {}

        class Instance : public Drawable
        {
        public:
            using Drawable::Drawable;
            
            // Drawable(memory_pool_type& pool, pointer instanceOf = nullptr)
            // Drawable(memory_pool_type& pool, const CoordFrame::pointer& parent, const glm::mat4& localPose = glm::mat4(1), pointer instanceOf = nullptr)
            // Drawable(memory_pool_type& pool, const std::string& name, const CoordFrame::pointer& parent = nullptr, const glm::mat4& localPose = glm::mat4(1), pointer instanceOf = nullptr)

            // template <typename... Args>
            // Instance(Drawable::memory_pool_type& pool, const Drawable::pointer& instanceOf, Args&&... args)
            //     : Drawable(pool, std::forward< Args >(args)..., instanceOf)
            // {}
            virtual ~Instance() {}
        };

        void setup() override
        {
            glEnable(GL_LINE_SMOOTH);
            glLineWidth(2);
            
            m_vertexBuffer = gl_classes::HostDeviceBuffer<glm::vec3>(GL_ARRAY_BUFFER, GL_STATIC_DRAW, 0, 3);
            m_vertexBuffer.init();
            m_vertexBuffer.upload();
            m_verticesChanged = false;

            m_colorBuffer = gl_classes::HostDeviceBuffer<glm::vec3>(GL_ARRAY_BUFFER, GL_STATIC_DRAW, 0, 3);
            m_colorBuffer.init();
            m_colorBuffer.upload();
            m_colorsChanged = false;

            m_pvmBuffer = gl_classes::HostDeviceBuffer<glm::mat4, gl_classes::NonShrinkingVector<glm::mat4>>(GL_ARRAY_BUFFER, GL_STATIC_DRAW, 0, 1);
            m_pvmBuffer.init();
            m_pvmBuffer.buffer.push_back(glm::mat4(1));
            m_pvmBuffer.upload();
            m_pvmChanged = false;

            checkGLError();
            m_vertexArray.init({
               gl_classes::VertexArray::VertexAttribPointer(m_vertexBuffer.getBufferId(), 3, GL_FLOAT, sizeof(float), GL_FALSE, 0, (void*)0, 0),
               gl_classes::VertexArray::VertexAttribPointer(m_colorBuffer.getBufferId(), 3, GL_FLOAT, sizeof(float), GL_FALSE, 0, (void*)0, 0),
               gl_classes::VertexArray::VertexAttribPointer(m_pvmBuffer.getBufferId(), 16, GL_FLOAT, sizeof(float), GL_FALSE, 0, (void*)0, 1)
            });
            // checkGLError();

        }

        void draw() override
        {
            //sortInstances();
            upload();
            if (m_pvmBuffer.size() == 0) return;
            bind();
            glDrawArraysInstanced(GL_LINES, 0, m_vertexBuffer.size(), m_pvmBuffer.size());  checkGLError();
            unbind();
            //std::cout << "m_pvmBuffer.buffer.size() " << m_pvmBuffer.buffer.size() << "\n";
            if (m_clearPvmOnDraw)      m_pvmBuffer.buffer.clear();
            if (m_clearVerticesOnDraw) m_vertexBuffer.buffer.clear();
            if (m_clearColorOnDraw)    m_colorBuffer.buffer.clear();
            // m_verticesChanged = true;
            // m_colorsChanged = true;
            // m_pvmChanged = true;
        }

        void clearInstances()
        {
            m_pvmBuffer.buffer.clear();
        }
        void clearLines()
        {
            m_vertexBuffer.buffer.clear();
            m_colorBuffer.buffer.clear();
        }

        void drawLine(const glm::vec3& from, const glm::vec3& to, const glm::vec3& fromColor, const glm::vec3& toColor)
        {
            m_vertexBuffer.buffer.push_back(from);
            m_vertexBuffer.buffer.push_back(to);
            m_colorBuffer.buffer.push_back(fromColor);
            m_colorBuffer.buffer.push_back(toColor);
            m_verticesChanged = true;
            m_colorsChanged = true;
        }

        void drawInstance(const glm::mat4& pvm) override
        {
            m_pvmBuffer.buffer.push_back(pvm);
            m_pvmChanged = true;
        }

        bool isInstancedRenderer() const override { return m_isInstanced; }
        bool clearPvmOnDraw()      const { return m_clearPvmOnDraw;       }
        bool clearVerticesOnDraw() const { return m_clearVerticesOnDraw;  }
        bool clearColorOnDraw()    const { return m_clearColorOnDraw;     }

        bool isInstancedRenderer(bool val) { m_isInstanced         = val; return val; }
        bool clearPvmOnDraw(bool val)      { m_clearPvmOnDraw      = val; return val; }
        bool clearVerticesOnDraw(bool val) { m_clearVerticesOnDraw = val; return val; }
        bool clearColorOnDraw(bool val)    { m_clearColorOnDraw    = val; return val; }

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

        void upload()
        {
            uploadVertices();
            uploadColors();
            uploadPvm();
        }
        void uploadVertices()
        {
            if (!m_verticesChanged) return;
            m_vertexBuffer.bind(); checkGLError();
            m_vertexBuffer.upload(); checkGLError();
            m_verticesChanged = false; 
        }
        void uploadColors()
        {
            if (!m_colorsChanged) return;
            m_colorBuffer.bind(); checkGLError();
            m_colorBuffer.upload(); checkGLError();
            m_colorsChanged = false; 
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
        gl_classes::HostDeviceBuffer<glm::vec3> m_vertexBuffer;
        gl_classes::HostDeviceBuffer<glm::vec3> m_colorBuffer;
        gl_classes::HostDeviceBuffer<glm::mat4, gl_classes::NonShrinkingVector<glm::mat4>> m_pvmBuffer;
        bool m_pvmChanged;
        bool m_verticesChanged;
        bool m_colorsChanged;

        bool m_clearPvmOnDraw = false;
        bool m_clearVerticesOnDraw = false;
        bool m_clearColorOnDraw = false;
        bool m_isInstanced = false;
    };

} // namespace drawing
} // namespace gui_application
