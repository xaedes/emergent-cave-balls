#pragma once

#include <glm/glm.hpp>
#include "gui_application/cameras/camera_view_interface.h"

// #include <glm/gtx/extend.hpp>
// #include <glm/gtx/transform.hpp> 
// #include <glm/gtx/norm.hpp>

#include <glm/ext/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::lookAt
#include <glm/ext/scalar_constants.hpp> // glm::pi

namespace gui_application {
namespace cameras {

    class LookAtView : public CameraViewInterface
    {
    public:

        LookAtView(glm::vec3 position, glm::vec3 target, glm::vec3 up)
            : position(position), target(target), up(up)
        {}
        LookAtView() : LookAtView(glm::vec3(0,0,0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 1))
        {}

        virtual ~LookAtView()
        {}

        operator glm::mat4() const override
        {
            return glm::lookAt(position, target, up);
        }

        glm::vec3 position;
        glm::vec3 target;
        glm::vec3 up = glm::vec3(0,0,1);
    };

} // namespace cameras
} // namespace gui_application
