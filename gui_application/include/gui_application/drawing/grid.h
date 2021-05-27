#pragma once

#include <glm/glm.hpp>
#include "gui_application/drawing/drawable.h"
#include "gl_classes/host_device_buffer.h"

namespace gui_application {
namespace drawing {

    class Grid : public Drawable
    {
    public:
        using Drawable::Drawable;
        using pointer = Grid*;

        //template <typename... Args>
        //Grid(Args&&... args)
        //    : Drawable(std::forward< Args >( args )...)
        //{}

        virtual ~Grid() {}

        void setup() override
        {}
        void draw() override
        {}

    protected:
        gl_classes::HostDeviceBuffer<glm::vec3> m_vertexBuffer;
        gl_classes::HostDeviceBuffer<glm::vec3> m_colorBuffer;
        gl_classes::HostDeviceBuffer<glm::mat4> m_pvmBuffer;

    };

} // namespace drawing
} // namespace gui_application
