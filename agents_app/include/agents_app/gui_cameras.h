#pragma once

#include <glm/glm.hpp>
#include "agents_app/version.h"
#include "gl_classes/imgui_gl.h"
#include "gui_application/drawing/drawable.h"
#include "gui_application/cameras/cameras.h"

namespace agents_app {

    //template <class App>
    class App;
    class GuiCameras
    {
    public:
        using Drawable = gui_application::drawing::Drawable;
        using Transform = typename Drawable::Transform;

        GuiCameras(App& app);
        virtual ~GuiCameras();
        void setup();
        void frame();

        const glm::mat4& projection_view() const { return m_projection_view; }
    protected:
        App& app;

        gui_application::cameras::FirstPersonPerspectiveCamera m_firstPersonCamera;
        gui_application::cameras::OrbitalPerspectiveCamera m_orbitalCamera;
        gui_application::cameras::LookAtPerspectiveCamera m_lookAtCamera;
        
        int m_activeCamera = 0;
        glm::mat4 m_projection_view;
    };

} // namespace agents_app
