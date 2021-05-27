#pragma once

#include <glm/glm.hpp>

namespace gui_application {
namespace cameras {

    class CameraProjectionInterface
    {
    public:
        CameraProjectionInterface(){}
        virtual ~CameraProjectionInterface(){}
        glm::mat4 mat() const { return *this; }
        virtual operator glm::mat4() const = 0;
        virtual void updateViewport(glm::ivec2 size) = 0;
    };

} // namespace cameras
} // namespace gui_application
