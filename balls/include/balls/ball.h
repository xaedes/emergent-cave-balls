#pragma once

#include "glm/glm.hpp"
#include "im_param/im_param.h"
#include "im_param/gl_classes/glm_specializations.h"
#include "geometry/pi.h"

namespace balls {

    struct Ball
    {
        glm::vec2 pos = glm::vec2(0,0);
        glm::vec2 vel = glm::vec2(0,0);
        glm::vec2 impulse = glm::vec2(0,0); 
        float radius = 1;

        // fields for 16byte alignment padding:
        // use these first when adding new actual fields
        float _1 = 0;
        // float _2 = 0;
        // float _3 = 0;
    };

} // namespace balls

namespace im_param {

    template <class backend_type>
    backend_type& parameter(
        backend_type& backend,
        ::balls::Ball& ball, 
        const TypeHolder<::balls::Ball>&)
    {
        parameter(backend, "pos", ball.pos);
        parameter(backend, "vel", ball.vel);
        parameter(backend, "impulse", ball.impulse);
        parameter(backend, "radius", ball.radius);
        parameter(backend, "_1", ball._1);
        return backend;
    }
    
} // namespace im_param
