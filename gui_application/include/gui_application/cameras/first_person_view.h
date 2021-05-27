#pragma once

#include <glm/glm.hpp>
#include "gui_application/cameras/camera_view_interface.h"

// #include <glm/gtx/extend.hpp>
// #include <glm/gtx/transform.hpp> 
// #include <glm/gtx/norm.hpp>

#include <glm/ext/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale
#include <glm/ext/scalar_constants.hpp> // glm::pi
#include <glm/gtc/matrix_inverse.hpp>   // glm::affineInverse
#include <glm/gtx/euler_angles.hpp>   // glm::eulerAngleX, glm::eulerAngleY, glm::eulerAngleZ
#include <glm/gtx/transform.hpp>   // glm::translate, glm::rotate, 
namespace gui_application {
namespace cameras {

    class FirstPersonView : public CameraViewInterface
    {
    public:

        FirstPersonView(glm::vec3 position, glm::vec3 rollPitchYaw)
            : position(position), rollPitchYaw(rollPitchYaw)
        {}
        FirstPersonView() : FirstPersonView(glm::vec3{}, glm::vec3{}) 
        {}

        virtual ~FirstPersonView()
        {}

        operator glm::mat4() const override
        {
            glm::mat4 world_camera = 
                glm::translate(position)
                * (glm::eulerAngleY(rollPitchYaw.z)) 
                * (glm::eulerAngleZ(rollPitchYaw.x))
                * (glm::eulerAngleX(rollPitchYaw.y))
                ;
            glm::mat4 camera_world = glm::affineInverse(world_camera);
            return camera_world;
        }

        glm::vec3 position;
        glm::vec3 rollPitchYaw;

        static constexpr glm::vec3 x_unit = glm::vec3(1,0,0);
        static constexpr glm::vec3 y_unit = glm::vec3(0,1,0);
        static constexpr glm::vec3 z_unit = glm::vec3(0,0,1);
    };

} // namespace cameras
} // namespace gui_application
