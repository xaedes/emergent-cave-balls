// #pragma once
#include "agents_app/simulation_clock.h"
#include "agents_app/app.h"
#include "gui_application/glm_to_string.h"

namespace agents_app {

SimulationClock::SimulationClock(App& app) 
    : app(app)
{

}

SimulationClock::~SimulationClock() 
{

}

void SimulationClock::setup()
{
    m_displayContainer = app.pool.create<Drawable>(app.pool, "SimulationClock", app.root);
    m_display = app.pool.create<Transform>("", *m_displayContainer);

    m_pauseManual = true;
    m_pauseForced = false;
    m_showAdvanced = false;
    m_minDt = 0;
    m_maxDt = 1;
    m_dt = 0;
    m_currentTime = 0;
    m_clockSpeed = 0;
    m_maxTime = 0;
    m_maxTimeIncreaseThreshold = 100;
    m_timeJumped = true;

}

void SimulationClock::rewind()
{
    m_currentTime = 0;
    m_timeJumped = true;
}

void SimulationClock::reset()
{
    m_dt = 0;
    m_currentTime = 0;
    m_clockSpeed = 0;
    m_maxTime = 0;
    m_timeJumped = true;
}

void SimulationClock::frame()
{
    while (m_maxTime - m_currentTime < m_maxTimeIncreaseThreshold)
    {
        m_maxTime += m_maxTimeIncreaseThreshold;
    }
    update();


    ImGui::Begin("SimulationClock");
    float currentTimeFloat = m_currentTime;
    std::string dtStr ="Time Delta: " + std::to_string(m_dt);
    float fps = (m_dt == 0) ? 0 : (1 / m_dt);
    std::string fpsStr ="FPS in Simulation: " + std::to_string(fps);

    ImGui::Text(dtStr.c_str());
    ImGui::Text(fpsStr.c_str());
    ImGui::Separator();

    if (ImGui::SliderFloat("Current Time", &currentTimeFloat, 0, m_maxTime))
    {
        m_currentTime = currentTimeFloat;
        m_timeJumped = true;
    }
    float clockSpeedFloat = m_clockSpeed;
    if (ImGui::SliderFloat("Clock Speed", &clockSpeedFloat, 0, 10))
    {
        m_clockSpeed = clockSpeedFloat;
    }
    ImGui::Separator();

    ImGui::Checkbox("Pause", &m_pauseManual);
    if (ImGui::Button("Reset"))
    {
        ImGui::OpenPopup("Confirm Reset");
        // m_pauseForced = true;
    }
    if (ImGui::BeginPopupModal("Confirm Reset", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Do you really want to reset the simulated clock?");
        ImGui::Separator();

        if (ImGui::Button("Confirm"))
        {
            reset();
            ImGui::CloseCurrentPopup();
            // m_pauseForced = false;
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
        {
            ImGui::CloseCurrentPopup();
            // m_pauseForced = false;
        }
        ImGui::SetItemDefaultFocus();
        ImGui::EndPopup();
    }
    ImGui::Separator();
    ImGui::Text("Display");
    float scale = m_displayContainer->transform.accessLocalScale().y;
    if (ImGui::SliderFloat("Time Display Scale", &scale, 1e-6, 10))
    {
        m_displayContainer->transform.accessLocalScale().y = scale;
    }
    

    ImGui::Separator();

    ImGui::Checkbox("Advanced Options", &m_showAdvanced);
    if (m_showAdvanced)
    {
        ImGui::InputFloat("Max Time", &m_maxTime);
        ImGui::InputFloat("Max Time Inc Threshold", &m_maxTimeIncreaseThreshold);
        float minDt = m_minDt;
        float maxDt = m_maxDt;
        float maxFps = (m_minDt == 0) ? 0 : (1 / m_minDt);
        float minFps = (m_maxDt == 0) ? 0 : (1 / m_maxDt);
        if (ImGui::InputFloat("Min Delta Time", &minDt) && (minDt >= 0)) { m_minDt = minDt; }
        if (ImGui::InputFloat("Max Delta Time", &maxDt) && (maxDt >= 0)) { m_maxDt = maxDt; }
        if (ImGui::InputFloat("Min FPS", &minFps) && (minFps > 0)) { m_maxDt = 1 / minFps; }
        if (ImGui::InputFloat("Max FPS", &maxFps) && (maxFps > 0)) { m_minDt = 1 / maxFps; }
    }
    ImGui::End();

    m_pauseForced = ImGui::IsPopupOpen(nullptr, ImGuiPopupFlags_AnyPopup);
    
}

void SimulationClock::update()
{
    if (!paused())
    {
        m_dt = m_clockSpeed * app.time.dt();
        if (m_dt < m_minDt) m_dt = m_minDt;
        if (m_dt > m_maxDt) m_dt = m_maxDt;
        m_currentTime += m_dt;
        m_timeJumped = false;
        
        m_display->accessLocalPosition().y = -m_currentTime;
    }
}



}
