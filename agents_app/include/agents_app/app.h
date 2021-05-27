#pragma once

#include <vector>

#include "gui_application/gui_application.h"
#include "gui_application/drawing/drawable.h"

#include "agents_app/version.h"
#include "agents_app/gui_cameras.h"
#include "agents_app/entity_browser.h"
#include "agents_app/renderer.h"
#include "agents_app/time.h"
#include "agents_app/simulation_clock.h"
#include "agents_app/random.h"

#include "agents_app/ants.h"
#include "agents_app/balls.h"

#include "imgui.h"

namespace agents_app {

    class App : public gui_application::GuiApplication
    {
    public:
        using memory_pool_type = typename gui_application::drawing::Drawable::memory_pool_type;
        using Drawable = gui_application::drawing::Drawable;
        using Transform = typename Drawable::Transform;
        
        App();
        virtual ~App();
        void setup() override;
        void frame() override;

        memory_pool_type    pool;
        Transform::pointer  root;
        Time                time;
        GuiCameras          cameras;
        EntityBrowser       entityBrowser;
        Renderer            renderer;
        SimulationClock     simulationClock;
        Random              random;
        
        Ants                ants;
        Balls               balls;
        
        
        
    protected:

    };

} // namespace agents_app
