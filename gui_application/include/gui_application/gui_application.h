#pragma once

#include "gl_classes/imgui_gl.h"
#include "gui_application/version.h"

#include <string>

namespace gui_application {

class GuiApplication
{
public:
    GuiApplication(bool enableGlDebugOutput=true);
    virtual ~GuiApplication();

    int run();

    virtual std::string name();
    virtual void setup();
    virtual void setupFonts();
    virtual void frame();
    virtual void teardown();

    GLFWwindow* window() const { return m_window; }
protected:
    bool m_enableGlDebugOutput;
    GLFWwindow* m_window;

};

} // namespace gui_application
