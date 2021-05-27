#pragma once

#include "gui_application/version.h"
#include "gui_application/gui_application.h"

#include "imgui.h"

namespace gui_application {

    class ImGuiDemo : public GuiApplication
    {
    public:
        ImGuiDemo() {}
        virtual ~ImGuiDemo() {}
        void frame() override
        {
            if (m_showDemoWindow)
            {
                ImGui::ShowDemoWindow(&m_showDemoWindow);
            }
            glBegin(GL_LINE_STRIP);
            glVertex3f(-100.0f, -100.0f, 0.0f);
            glVertex3f(+100.0f, -100.0f, 0.0f);
            glVertex3f(+100.0f, +100.0f, 0.0f);
            glVertex3f(-100.0f, +100.0f, 0.0f);
            glVertex3f(-100.0f, -100.0f, 0.0f);
            glEnd();
        }
    protected:

        bool m_showDemoWindow;
    };

} // namespace gui_application
