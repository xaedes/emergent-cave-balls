#pragma once

#include "agents_app/version.h"
#include "gui_application/drawing/drawable.h"
#include "gl_classes/imgui_gl.h"

namespace agents_app {

    class App;
    class EntityBrowser
    {
    public:
        using Drawable = gui_application::drawing::Drawable;
        using Transform = typename Drawable::Transform;
        
        EntityBrowser(App& app);
        virtual ~EntityBrowser();
        void setup();
        void frame();
        
    protected:
        void imgui_coord_frame(Transform* tf);
        App& app;
    };

} // namespace agents_app

