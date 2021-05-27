
#include <iostream>
#include "gui_application/version.h"

#include "gui_application/gui_application.h"

int main()
{
    std::cout << "gui_application::Version " << gui_application::VersionMajor << "." << gui_application::VersionMinor << "." << gui_application::VersionPatch << std::endl;
    gui_application::GuiApplication app;
    return app.run();
}