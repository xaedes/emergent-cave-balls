#pragma once

#include "glm/glm.hpp"
#include "im_param/im_param.h"
#include "im_param/gl_classes/glm_specializations.h"
#include "geometry/pi.h"

namespace ants {

    struct Ant
    {
        glm::vec4  pos_vel                  = glm::vec4(0,0,0,0);
        glm::vec4  carry_head_focus_despair = glm::vec4(0,0,PI/2,0);
        glm::vec4  homedist_fooddist_rndstr = glm::vec4(0,0,PI/2,0);
        glm::vec4  maxage_age_grip          = glm::vec4(0,0,0,0);
        glm::ivec4 state                    = glm::ivec4(0,0,0,0);
        glm::uvec4 random                   = glm::uvec4(0,0,0,0);
        glm::vec4  environment_delta        = glm::vec4(0,0,0,0);
        glm::vec4  display_value            = glm::vec4(0,0,0,0);
    };

} // namespace ants

namespace im_param {

    template <class backend_type>
    backend_type& parameter(
        backend_type& backend,
        ::ants::Ant& ant, 
        const TypeHolder<::ants::Ant>&)
    {
        parameter(backend, "pos_vel",                  ant.pos_vel                  );
        parameter(backend, "carry_head_focus_despair", ant.carry_head_focus_despair );
        parameter(backend, "homedist_fooddist_rndstr", ant.homedist_fooddist_rndstr );
        parameter(backend, "maxage_age_grip",          ant.maxage_age_grip          );
        parameter(backend, "state",                    ant.state                    );
        parameter(backend, "random",                   ant.random                   );
        parameter(backend, "environment_delta",        ant.environment_delta        );
        parameter(backend, "display_value",            ant.display_value            );
        return backend;
    }
    
} // namespace im_param
