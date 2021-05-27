#pragma once

#include <type_traits>
#include <glm/glm.hpp>
#include "gui_application/cameras/camera_view_interface.h"
#include "gui_application/cameras/camera_projection_interface.h"

namespace gui_application {
namespace cameras {

    template<
        typename projection_type,
        typename view_type,
        // https://stackoverflow.com/a/30687399/798588
        typename std::enable_if<std::is_base_of<CameraProjectionInterface, projection_type>::value>::type* = nullptr,
        typename std::enable_if<std::is_base_of<CameraViewInterface, view_type>::value>::type* = nullptr
    >
    class Camera
    {
    public:
        Camera(){}
        virtual ~Camera(){}
        virtual void updateViewport(glm::ivec2 size)
        {
            projection.updateViewport(size);
        }
        
        projection_type projection;
        view_type view;
    };

} // namespace cameras
} // namespace gui_application
