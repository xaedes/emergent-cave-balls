
#include <iostream>
#include "agents_app/version.h"
#include "agents_app/app.h"

int main(int argc, char **argv)
{   
    std::cout << "agents_app::Version " << agents_app::VersionMajor << "." << agents_app::VersionMinor << "." << agents_app::VersionPatch << std::endl;
    agents_app::App app;
    return app.run();
}