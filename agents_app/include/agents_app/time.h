#pragma once

#include "agents_app/version.h"
#include "gui_application/drawing/drawable.h"
#include "gl_classes/imgui_gl.h"
#include <chrono>

namespace agents_app {

    class App;
    class Time
    {
    public:
        using Drawable = gui_application::drawing::Drawable;
        using Transform = typename Drawable::Transform;
        
        Time(App& app);
        virtual ~Time();
        void setup();
        void frame();
        

        inline double dt() const { return m_dt; }
        inline double secondsSinceStart() const { return m_secondsSinceStart; }
        inline std::chrono::time_point<std::chrono::high_resolution_clock> start() const { return m_start; }
        inline std::chrono::time_point<std::chrono::high_resolution_clock> now() const { return m_now; }
        inline std::chrono::time_point<std::chrono::high_resolution_clock> lastTime() const { return m_lastTime2; }
    protected:
        App& app;
        double m_dt;
        double m_secondsSinceStart;
        std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
        std::chrono::time_point<std::chrono::high_resolution_clock> m_now;
        std::chrono::time_point<std::chrono::high_resolution_clock> m_lastTime;
        std::chrono::time_point<std::chrono::high_resolution_clock> m_lastTime2;

    };

} // namespace agents_app

