#pragma once

#include "agents_app/version.h"
#include "gui_application/drawing/drawable.h"
#include "gui_application/shader/default_program.h"
#include "gl_classes/imgui_gl.h"

namespace agents_app {

    class App;
    class Renderer
    {
    public:
        using Drawable = gui_application::drawing::Drawable;
        using Transform = typename Drawable::Transform;
        using DefaultProgram = gui_application::shader::DefaultProgram;
        
        Renderer(App& app);
        virtual ~Renderer();
        void setup();
        void frame();
        
    protected:
        App& app;
        DefaultProgram m_drawProgram;
        std::vector<glm::mat4> m_poseTrace;
    };

} // namespace agents_app

