
#include <catch2/catch.hpp>

#include "gui_application/version.h"
#include "gui_application/cameras/cameras.h"



TEST_CASE("gui_application::cameras::FirstPersonView", "[gui_application][cameras][FirstPersonView]")
{
    using FirstPersonView = gui_application::cameras::FirstPersonView;

    SECTION("camera in origin with all zero orientation")
    {
        auto X = GENERATE(-2, -1, -0.5, 0, 0.5, 1, 2);
        auto Y = GENERATE(-2, -1, -0.5, 0, 0.5, 1, 2);
        auto Z = GENERATE(-2, -1, -0.5, 0, 0.5, 1, 2);
        glm::vec3 position(0, 0, 0);
        glm::vec3 rollPitchYaw(0, 0, 0);
        glm::vec4 worldCoordinate(X, Y, Z, 1);
        FirstPersonView view(position, rollPitchYaw);
        glm::mat4 mat = view;
        glm::vec4 viewCoordinate = mat * worldCoordinate;
        REQUIRE(viewCoordinate.x == X);
        REQUIRE(viewCoordinate.y == Y);
        REQUIRE(viewCoordinate.z == Z);
    }

    SECTION("camera translated with all zero orientation")
    {
        auto TX = GENERATE(-2, -1, -0.5, 0, 0.5, 1, 2);
        auto TY = GENERATE(-2, -1, -0.5, 0, 0.5, 1, 2);
        auto TZ = GENERATE(-2, -1, -0.5, 0, 0.5, 1, 2);
        auto X = GENERATE(-2, -1, -0.5, 0, 0.5, 1, 2);
        auto Y = GENERATE(-2, -1, -0.5, 0, 0.5, 1, 2);
        auto Z = GENERATE(-2, -1, -0.5, 0, 0.5, 1, 2);
        glm::vec3 position(TX, TY, TZ);
        glm::vec3 rollPitchYaw(0, 0, 0);
        glm::vec4 worldCoordinate(X, Y, Z, 1);
        FirstPersonView view(position, rollPitchYaw);
        glm::mat4 mat = view;
        glm::vec4 viewCoordinate = mat * worldCoordinate;
        REQUIRE(viewCoordinate.x == X-TX);
        REQUIRE(viewCoordinate.y == Y-TY);
        REQUIRE(viewCoordinate.z == Z-TZ);
    }

    
}
