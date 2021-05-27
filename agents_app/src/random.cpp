// #pragma once
#include "agents_app/random.h"
#include "agents_app/app.h"

namespace agents_app {

Random::Random(App& app) 
    : app(app)
    , gui_application::Random(1337)
{}

Random::~Random() 
{}

void Random::setup()
{
    gui_application::Random::seed(1337);
}

void Random::frame()
{}

}
