
#include <iostream>
#include "gui_application/version.h"

#include "gui_application/cube_demo.h"



int main()
{
    std::cout << "gui_application::Version " << gui_application::VersionMajor << "." << gui_application::VersionMinor << "." << gui_application::VersionPatch << std::endl;
    gui_application::CubeDemo app;
    return app.run();
}

