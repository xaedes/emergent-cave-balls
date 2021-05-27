#include "agents_app/app.h"

namespace agents_app {
    
App::App() 
    : gui_application::GuiApplication(true)
    , time(*this)
    , cameras(*this)
    , entityBrowser(*this)
    , renderer(*this)
    , simulationClock(*this)
    , random(*this)
    , ants(*this)
    , balls(*this)
{

}

App::~App() 
{

}

void App::setup()
{
    root = pool.create<Transform>("root");
    time.setup();
    simulationClock.setup();
    cameras.setup();
    entityBrowser.setup();
    renderer.setup();
    random.setup();
    // ants.setup();
    balls.setup();
    
    for (auto it = root->begin_recurse_data<Drawable>(); it != root->end_recurse_data<Drawable>(); ++it)
    {
        Drawable* drawable = static_cast<Drawable*>(it);
        if(drawable) drawable->setup();
    }
}

void App::frame()
{
    time.frame();
    simulationClock.frame();
    cameras.frame();
    entityBrowser.frame();
    renderer.frame();
    random.frame();
    // ants.frame();
    balls.frame();
}

} // namespace agents_app
