#include "agents_app/ants.h"
#include "agents_app/app.h"
#include "gui_application/glm_to_string.h"


#include "im_param/backends/imgui_backend.h"
#include "im_param/backends/glsl_struct_generator_backend.h"


namespace agents_app {

Ants::Ants(App& app) 
    : app(app)
{

}

void Ants::setup()
{
    m_ants.params.width = 1024;
    m_ants.params.height = 1024;

    m_ants.setup(static_cast<gui_application::Random&>(app.random));

    checkGLError();

    m_quadEnvironment = app.pool.create<TextureQuad>(glm::vec2(m_ants.params.width, m_ants.params.height), app.pool, "enviroment", app.root);
    m_quadEnvironment->setup();

    m_quadDisplay = app.pool.create<TextureQuad>(glm::vec2(m_ants.params.width, m_ants.params.height), app.pool, "display", app.root);
    m_quadDisplay->setup();

    m_quadEnvironment->textureId = m_ants.m_texIdsEnv.read();
    m_quadDisplay->textureId = m_ants.m_texIdsDisplay.read();

    checkGLError();
}



void Ants::frame()
{
    if (!app.simulationClock.paused())
    {
        m_ants.update(app.simulationClock.dt());
        m_quadEnvironment->textureId = m_ants.m_texIdsEnv.read();
        m_quadDisplay->textureId = m_ants.m_texIdsDisplay.read();
    }

    ImGui::Begin("Ants");

    //ImGui::Text(std::to_string(nonZero).c_str());

    if (ImGui::CollapsingHeader("Show"))
    {
        ImGui::Checkbox("Enable Environment", &m_quadEnvironment->enabled());
        ImGui::Checkbox("Enable Display", &m_quadDisplay->enabled());
    }
    im_param::ImGuiBackend imguiBackend; 
    im_param::parameter(imguiBackend, "Ants", m_ants, im_param::TypeHolder<decltype(m_ants)>(), im_param::HierarchyType::CollapsingHeader);

    if(imguiBackend.changed)
    {
        m_ants.resizeAnts(m_ants.m_antsProgram.num_items.get());
    }
    if (ImGui::CollapsingHeader("Clear"))
    {
        if (ImGui::Button("Clear All"))
        {
            m_ants.clear();
        }
        if (ImGui::Button("Clear Environment"))
        {
            m_ants.clearEnvironment();
        }
        if (ImGui::Button("Clear Display"))
        {
            m_ants.clearDisplay();
        }
        if (ImGui::Button("Reset Ants"))
        {
            m_ants.resetAnts();
        }
    }

    ImGui::End();
}



}
