#pragma once

#include <random>

#include "agents_app/version.h"
#include "gui_application/random.h"

namespace agents_app {

    class App;
    class Random : public gui_application::Random
    {
    public:
        Random(App& app);
        virtual ~Random();
        void setup();
        void frame();
        
    protected:
        App& app;
    };

} // namespace agents_app

