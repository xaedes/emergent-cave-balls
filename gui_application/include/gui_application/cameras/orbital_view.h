#pragma once

#include <glm/glm.hpp>
#include "gui_application/cameras/camera_view_interface.h"

#include <glm/gtx/extend.hpp>
#include <glm/gtx/transform.hpp> 
#include <glm/gtx/norm.hpp>
#include <glm/gtx/rotate_vector.hpp> // glm::rotateX

#include <glm/ext/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::lookAt
#include <glm/ext/scalar_constants.hpp> // glm::pi
#include <glm/gtc/matrix_inverse.hpp>   // glm::affineInverse
#include <glm/gtx/euler_angles.hpp>   // glm::eulerAngleX, glm::eulerAngleY, glm::eulerAngleZ
#include <glm/gtx/transform.hpp>   // glm::translate, glm::rotate, 

namespace gui_application {
namespace cameras {

    class OrbitalView : public CameraViewInterface
    {
    public:

        OrbitalView(glm::vec3 target, glm::vec3 angles, float distance)
            : target(target), angles(angles), distance(distance)
        {}
        OrbitalView() : OrbitalView(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), 0)
        {}

        virtual ~OrbitalView()
        {}

        operator glm::mat4() const override
        {
            glm::mat4 world_camera =
                glm::translate(target)
                * (glm::eulerAngleY(angles.z))
                * (glm::eulerAngleX(-angles.y))
                * (glm::eulerAngleZ(angles.x))
                * glm::translate(glm::vec3(0, 0, distance))
                
                ;
            glm::mat4 camera_world = glm::affineInverse(world_camera);
            //glm::mat4 camera_world = 
            return camera_world;
        }

        glm::vec3 position() const
        {
            glm::mat4 mat = *this;
            return mat * glm::vec4(0, 0, 0, 1);
        }

        glm::vec3 target;
        glm::vec3 angles;
        float distance;

        static constexpr glm::vec3 x_unit = glm::vec3(1,0,0);
        static constexpr glm::vec3 y_unit = glm::vec3(0,1,0);
        static constexpr glm::vec3 z_unit = glm::vec3(0,0,1);

    };

} // namespace cameras
} // namespace gui_application
