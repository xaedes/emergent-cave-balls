#pragma once

#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include "gui_application/cameras/camera_projection_interface.h"

namespace gui_application {
namespace cameras {

    class PerspectiveProjection : public CameraProjectionInterface
    {
    public:
        PerspectiveProjection():PerspectiveProjection(glm::radians(45.0f), 1e-3f, 1e5f, glm::ivec2(2,2)) {}

        PerspectiveProjection(float fov, float nearPlane, float farPlane, glm::ivec2 viewportSize)
            : fov(fov)
            , nearPlane(nearPlane)
            , farPlane(farPlane)
            , viewportSize(viewportSize)
        {}
        virtual ~PerspectiveProjection()
        {}

        operator glm::mat4() const override
        {
            return glm::perspective(fov, aspectRatio(), nearPlane, farPlane);
        }

        void updateViewport(glm::ivec2 size) override
        {
            viewportSize = size;
        }

        float fov;
        float nearPlane;
        float farPlane;
        glm::ivec2 viewportSize; 

        float aspectRatio() const
        {
            if (viewportSize[1] == 0) 
            {
                return 1.0f;
            }
            else 
            {
                return (viewportSize[0] / static_cast<float>(viewportSize[1]));
            }
        }
    };

} // namespace cameras
} // namespace gui_application
