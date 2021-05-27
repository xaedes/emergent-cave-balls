#pragma once

#include <glm/glm.hpp>

namespace gui_application {
namespace cameras {

    class CameraViewInterface
    {
    public:
        CameraViewInterface(){}
        virtual ~CameraViewInterface(){}
        glm::mat4 mat() const { return *this; }
        virtual operator glm::mat4() const = 0;
    };

} // namespace cameras
} // namespace gui_application
