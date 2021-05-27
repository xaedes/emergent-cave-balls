#pragma once

#include "agents_app/version.h"
#include "gui_application/drawing/drawable.h"
#include "gui_application/drawing/texture_quad.h"
#include "gl_classes/imgui_gl.h"
#include "glm/glm.hpp"
#include "geometry/pi.h"

#include "ants/ants.h"

#include <string>

namespace agents_app {

    class App;
    class Ants
    {
    public:
        using Drawable = gui_application::drawing::Drawable;
        using TextureQuad = gui_application::drawing::TextureQuad;
        using Transform = typename Drawable::Transform;

        Ants(App& app);
        virtual ~Ants(){}
        void setup();
        void frame();



    protected:
        App& app;

        ants::Ants m_ants;
        TextureQuad* m_quadEnvironment;
        TextureQuad* m_quadDisplay;

    };

} // namespace agents_app

