#pragma once

#include "gui_application/cameras/camera.h"
#include "gui_application/cameras/first_person_view.h"
#include "gui_application/cameras/look_at_view.h"
#include "gui_application/cameras/orbital_view.h"
#include "gui_application/cameras/perspective_projection.h"

namespace gui_application {
namespace cameras {

    using FirstPersonPerspectiveCamera = Camera<PerspectiveProjection, FirstPersonView>;
    using LookAtPerspectiveCamera = Camera<PerspectiveProjection, LookAtView>;
    using OrbitalPerspectiveCamera = Camera<PerspectiveProjection, OrbitalView>;

} // namespace cameras
} // namespace gui_application
