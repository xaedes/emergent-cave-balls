#pragma once

#include <array>
#include <glm/glm.hpp>
#include "gui_application/drawing/drawable.h"

namespace gui_application {
namespace drawing {

    class Cube : public Drawable
    {
    public:
        using Drawable::Drawable;
        using pointer = Cube*;
        virtual ~Cube() {}

        void setup() override
        {
            glGenVertexArrays(1, &m_vertexArray);
            glBindVertexArray(m_vertexArray);

            // This will identify our vertex buffer
            // Generate 1 buffer, put the resulting identifier in vertexbuffer
            glGenBuffers(1, &m_vertexBuffer);
            // The following commands will talk about our 'vertexbuffer' buffer
            glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
            // Give our vertices to OpenGL.
            glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertexBufferData), m_vertexBufferData, GL_STATIC_DRAW);

            // This will identify our vertex buffer
            // Generate 1 buffer, put the resulting identifier in vertexbuffer
            glGenBuffers(1, &m_colorBuffer);
            // The following commands will talk about our 'vertexbuffer' buffer
            glBindBuffer(GL_ARRAY_BUFFER, m_colorBuffer);
            // Give our vertices to OpenGL.
            glBufferData(GL_ARRAY_BUFFER, sizeof(m_colorBufferData), m_colorBufferData, GL_STATIC_DRAW);

            // This will identify our instanced projectionViewModel buffer
            // Generate 1 buffer, put the resulting identifier in vertexbuffer
            glGenBuffers(1, &m_pvmBuffer);
            // The following commands will talk about our 'vertexbuffer' buffer
            glBindBuffer(GL_ARRAY_BUFFER, m_pvmBuffer);
            // Give identity matrix as instanced pvm to OpenGL.
            glBufferData(GL_ARRAY_BUFFER, 16*sizeof(float), &m_identity[0][0], GL_STATIC_DRAW);

            // 1st attribute buffer : vertices
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
            glVertexAttribPointer(
               0,                  // attribute 0. vertex
               3,                  // size
               GL_FLOAT,           // type
               GL_FALSE,           // normalized?
               0,                  // stride
               (void*)0            // array buffer offset
            );
            glEnableVertexAttribArray(1);
            glBindBuffer(GL_ARRAY_BUFFER, m_colorBuffer);
            glVertexAttribPointer(
               1,                  // attribute 1. color
               3,                  // size
               GL_FLOAT,           // type
               GL_FALSE,           // normalized?
               0,                  // stride
               (void*)0            // array buffer offset
            );

            // per instance projectionViewModel
            // glVertexAttribPointer only allows 4 floats size, so we need to pass each column seperate
            // https://gamedev.stackexchange.com/a/149561/14704
            glBindBuffer(GL_ARRAY_BUFFER, m_pvmBuffer);
            for (int i=0; i<4; ++i)
            {
                glEnableVertexAttribArray(2+i);
                glVertexAttribPointer(
                   2+i,                  // attribute 2. projectionViewModelInstances
                   4,                    // size
                   GL_FLOAT,             // type
                   GL_FALSE,             // normalized?
                   16 * sizeof(float),   // stride
                   (void*)(i * 4 * sizeof(float)) // array buffer offset
                ); checkGLError();
                glVertexAttribDivisor(2+i, 1);   // Specify the number of instances (1) that will pass between updates of the generic attribute at slot index 2
            }
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);

        }
        void draw() override
        {
            bind();
            glDrawArraysInstanced(GL_TRIANGLES, 0, 12 * 3, 1);
            unbind();
        }

        void bindProjectionViewModelInstances(const glm::mat4* projectionViewModelInstances, int count)
        {
            glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(m_identity[0][0]), &m_identity[0][0], GL_STATIC_DRAW);
            glBufferSubData( GL_ARRAY_BUFFER, 0, 16 * count * sizeof(float), projectionViewModelInstances );
        }

        inline GLuint vertexArray() const { return m_vertexArray; }
        inline GLuint vertexBuffer() const { return m_vertexBuffer; }
        inline GLuint colorBuffer() const { return m_colorBuffer; }
        inline GLuint pvmBuffer() const { return m_pvmBuffer; }
    protected:
        void bind()
        {
            glBindVertexArray(m_vertexArray);
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
            glVertexAttribPointer(
               0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
               3,                  // size
               GL_FLOAT,           // type
               GL_FALSE,           // normalized?
               0,                  // stride
               (void*)0            // array buffer offset
            );
            glEnableVertexAttribArray(1);
            glBindBuffer(GL_ARRAY_BUFFER, m_colorBuffer);
            glVertexAttribPointer(
               1,                  // attribute 1. No particular reason for 1, but must match the layout in the shader.
               3,                  // size
               GL_FLOAT,           // type
               GL_FALSE,           // normalized?
               0,                  // stride
               (void*)0            // array buffer offset
            );
            glBindBuffer(GL_ARRAY_BUFFER, m_pvmBuffer);
            for (int i=0; i<4; ++i)
            {
                glEnableVertexAttribArray(2+i);
                glVertexAttribPointer(
                   2+i,                  // attribute 2. projectionViewModelInstances
                   4,                    // size
                   GL_FLOAT,             // type
                   GL_FALSE,             // normalized?
                   16 * sizeof(float),   // stride
                   (void*)(i * 4 * sizeof(float)) // array buffer offset
                );
                glVertexAttribDivisor(2+i, 1);   // Specify the number of instances (1) that will pass between updates of the generic attribute at slot index 2
            }            
        }

        void unbind()
        {
            glDisableVertexAttribArray(0);
            glDisableVertexAttribArray(1);   
            glDisableVertexAttribArray(2);   
            glDisableVertexAttribArray(3);   
            glDisableVertexAttribArray(4);   
            glDisableVertexAttribArray(5);   
        }

        GLuint m_vertexArray;
        GLuint m_vertexBuffer;
        GLuint m_colorBuffer;
        GLuint m_pvmBuffer;


        const glm::mat4 m_identity = glm::mat4(1);

        // http://www.opengl-tutorial.org/beginners-tutorials/tutorial-4-a-colored-cube/
        // Our vertices. Three consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
        // A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices
        static constexpr GLfloat m_vertexBufferData[] = {
            -1.0f, -1.0f, -1.0f, // triangle 1 : begin
            -1.0f, -1.0f, +1.0f,
            -1.0f, +1.0f, +1.0f, // triangle 1 : end
            +1.0f, +1.0f, -1.0f, // triangle 2 : begin
            -1.0f, -1.0f, -1.0f,
            -1.0f, +1.0f, -1.0f, // triangle 2 : end
            +1.0f, -1.0f, +1.0f,
            -1.0f, -1.0f, -1.0f,
            +1.0f, -1.0f, -1.0f,
            +1.0f, +1.0f, -1.0f,
            +1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, +1.0f, +1.0f,
            -1.0f, +1.0f, -1.0f,
            +1.0f, -1.0f, +1.0f,
            -1.0f, -1.0f, +1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, +1.0f, +1.0f,
            -1.0f, -1.0f, +1.0f,
            +1.0f, -1.0f, +1.0f,
            +1.0f, +1.0f, +1.0f,
            +1.0f, -1.0f, -1.0f,
            +1.0f, +1.0f, -1.0f,
            +1.0f, -1.0f, -1.0f,
            +1.0f, +1.0f, +1.0f,
            +1.0f, -1.0f, +1.0f,
            +1.0f, +1.0f, +1.0f,
            +1.0f, +1.0f, -1.0f,
            -1.0f, +1.0f, -1.0f,
            +1.0f, +1.0f, +1.0f,
            -1.0f, +1.0f, -1.0f,
            -1.0f, +1.0f, +1.0f,
            +1.0f, +1.0f, +1.0f,
            -1.0f, +1.0f, +1.0f,
            +1.0f, -1.0f, +1.0f
        };
        static constexpr GLfloat m_colorBufferData[] = {
             0.0f,  0.0f,  0.0f, // triangle 1 : begin
             0.0f,  0.0f, +1.0f,
             0.0f, +1.0f, +1.0f, // triangle 1 : end
            +1.0f, +1.0f,  0.0f, // triangle 2 : begin
             0.0f,  0.0f,  0.0f,
             0.0f, +1.0f,  0.0f, // triangle 2 : end
            +1.0f,  0.0f, +1.0f,
             0.0f,  0.0f,  0.0f,
            +1.0f,  0.0f,  0.0f,
            +1.0f, +1.0f,  0.0f,
            +1.0f,  0.0f,  0.0f,
             0.0f,  0.0f,  0.0f,
             0.0f,  0.0f,  0.0f,
             0.0f, +1.0f, +1.0f,
             0.0f, +1.0f,  0.0f,
            +1.0f,  0.0f, +1.0f,
             0.0f,  0.0f, +1.0f,
             0.0f,  0.0f,  0.0f,
             0.0f, +1.0f, +1.0f,
             0.0f,  0.0f, +1.0f,
            +1.0f,  0.0f, +1.0f,
            +1.0f, +1.0f, +1.0f,
            +1.0f,  0.0f,  0.0f,
            +1.0f, +1.0f,  0.0f,
            +1.0f,  0.0f,  0.0f,
            +1.0f, +1.0f, +1.0f,
            +1.0f,  0.0f, +1.0f,
            +1.0f, +1.0f, +1.0f,
            +1.0f, +1.0f,  0.0f,
             0.0f, +1.0f,  0.0f,
            +1.0f, +1.0f, +1.0f,
             0.0f, +1.0f,  0.0f,
             0.0f, +1.0f, +1.0f,
            +1.0f, +1.0f, +1.0f,
             0.0f, +1.0f, +1.0f,
            +1.0f,  0.0f, +1.0f
        };
    };

} // namespace drawing
} // namespace gui_application
