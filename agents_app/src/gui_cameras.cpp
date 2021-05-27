#pragma once
#include "agents_app/gui_cameras.h"
#include "agents_app/app.h"
#include <glm/glm.hpp>
#include <string>

namespace agents_app {

GuiCameras::GuiCameras(App& app) 
    : app(app)
    , m_projection_view(glm::mat4(1))
{
}

GuiCameras::~GuiCameras() 
{

}

void GuiCameras::setup()
{
    m_activeCamera = 1;
    m_orbitalCamera.view.distance = 36;
}

void GuiCameras::frame()
{
    int display_w, display_h;
    glfwGetFramebufferSize(app.window(), &display_w, &display_h);

    ImGui::Begin("Camera");

    ImGui::BeginTabBar("Camera Options");
    bool firstPersonCameraOpen = m_activeCamera == 0;
    bool orbitalCameraOpen = m_activeCamera == 1;
    bool lookAtCameraOpen = m_activeCamera == 2;
    if (ImGui::BeginTabItem("Orbital Camera"))
    {
        ImGui::SliderFloat("target.x", &m_orbitalCamera.view.target.x, -1500, 1500);
        ImGui::SliderFloat("target.y", &m_orbitalCamera.view.target.y, -1500, 1500);
        ImGui::SliderFloat("target.z", &m_orbitalCamera.view.target.z, -1500, 1500);
        ImGui::SliderFloat("roll", &m_orbitalCamera.view.angles.x, -3.14159 * 2, 3.14159 * 2);
        ImGui::SliderFloat("pitch", &m_orbitalCamera.view.angles.y, -3.14159 * 2, 3.14159 * 2);
        ImGui::SliderFloat("yaw", &m_orbitalCamera.view.angles.z, -3.14159 * 2, 3.14159 * 2);
        ImGui::SliderFloat("distance", &m_orbitalCamera.view.distance, -200, 1500);
        ImGui::EndTabItem();
        m_activeCamera = 1;
    }
    if (ImGui::BeginTabItem("First Person Camera"))
    {
        ImGui::SliderFloat("position.x", &m_firstPersonCamera.view.position.x, -100, 100);
        ImGui::SliderFloat("position.y", &m_firstPersonCamera.view.position.y, -100, 100);
        ImGui::SliderFloat("position.z", &m_firstPersonCamera.view.position.z, -100, 100);
        ImGui::SliderFloat("roll", &m_firstPersonCamera.view.rollPitchYaw.x, -3.14159 * 2, 3.14159 * 2);
        ImGui::SliderFloat("pitch", &m_firstPersonCamera.view.rollPitchYaw.y, -3.14159 * 2, 3.14159 * 2);
        ImGui::SliderFloat("yaw", &m_firstPersonCamera.view.rollPitchYaw.z, -3.14159 * 2, 3.14159 * 2);
        ImGui::EndTabItem();
        m_activeCamera = 0;
    }
    if (ImGui::BeginTabItem("Look At Camera"))
    {
        ImGui::SliderFloat("position.x", &m_lookAtCamera.view.position.x, -100, 100);
        ImGui::SliderFloat("position.y", &m_lookAtCamera.view.position.y, -100, 100);
        ImGui::SliderFloat("position.z", &m_lookAtCamera.view.position.z, -100, 100);
        ImGui::SliderFloat("target.x", &m_lookAtCamera.view.target.x, -100, 100);
        ImGui::SliderFloat("target.y", &m_lookAtCamera.view.target.y, -100, 100);
        ImGui::SliderFloat("target.z", &m_lookAtCamera.view.target.z, -100, 100);
        ImGui::EndTabItem();
        m_activeCamera = 2;
    }
    ImGui::EndTabBar();
    ImGui::End();

    switch (m_activeCamera)
    {
    case 0:
        m_firstPersonCamera.updateViewport({ display_w, display_h });
        m_projection_view = m_firstPersonCamera.projection.mat() * m_firstPersonCamera.view.mat();
        break;
    case 1:
        m_orbitalCamera.updateViewport({ display_w, display_h });
        m_projection_view = m_orbitalCamera.projection.mat() * m_orbitalCamera.view.mat();
        break;
    case 2:
        m_lookAtCamera.updateViewport({ display_w, display_h });
        m_projection_view = m_lookAtCamera.projection.mat() * m_lookAtCamera.view.mat();
        break;
    }
}



}

