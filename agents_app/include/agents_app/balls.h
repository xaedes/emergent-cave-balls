#pragma once

#include "agents_app/version.h"
#include "gui_application/drawing/drawable.h"
#include "gui_application/drawing/texture_quad.h"
#include "gl_classes/imgui_gl.h"
#include "glm/glm.hpp"
#include "geometry/pi.h"

#include "balls/balls.h"

#include "im_param/im_param.h"

#include <string>

namespace agents_app {

    class App;
    class Balls
    {
    public:
        using Drawable = gui_application::drawing::Drawable;
        using TextureQuad = gui_application::drawing::TextureQuad;
        using Transform = typename Drawable::Transform;

        Balls(App& app);
        virtual ~Balls(){}
        void setup();
        void frame();

        struct Params
        {
            bool enabled;
        };
        Params params;
        balls::Balls balls;

    protected:
        App& app;

    };

} // namespace agents_app

namespace im_param {


    template <class backend_type>
    backend_type& parameter(
        backend_type& backend,
        ::agents_app::Balls::Params& params,
        const TypeHolder<::agents_app::Balls>&)
    {
        parameter(backend, "enabled", params.enabled);
        return backend;
    }

    template <class backend_type>
    backend_type& parameter(
        backend_type& backend,
        ::agents_app::Balls& balls,
        const TypeHolder<::agents_app::Balls>& th)
    {
        parameter(backend, "Parameters", balls.params, th);
        parameter(backend, "Balls", balls.balls, TypeHolder<::balls::Balls>());
        return backend;
    }

} // namespace im_param
