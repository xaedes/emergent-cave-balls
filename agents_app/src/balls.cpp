#include "agents_app/balls.h"
#include "agents_app/app.h"
#include "gui_application/glm_to_string.h"


#include "im_param/backends/imgui_backend.h"
#include "im_param/backends/glsl_struct_generator_backend.h"


namespace agents_app {

Balls::Balls(App& app) 
    : app(app)
{

}

void Balls::setup()
{
    // balls.params.width = 1024;
    // balls.params.height = 1024;

    balls.setup(static_cast<gui_application::Random&>(app.random));

    checkGLError();

    checkGLError();
}



void Balls::frame()
{
    if (!app.simulationClock.paused() && params.enabled)
    {
        balls.update(app.simulationClock.dt());
    }
    balls.draw(app.cameras.projection_view());

    ImGui::Begin("Balls");

    //ImGui::Text(std::to_string(nonZero).c_str());
    {
        auto str = std::string("num_interactions: ") + std::to_string(balls.stats.num_interactions);
        ImGui::Text(str.c_str());
    }

    im_param::ImGuiBackend imguiBackend; 
    im_param::parameter(imguiBackend, *this, im_param::TypeHolder<Balls>());

    if(imguiBackend.changed)
    {
        balls.parameterUpdate();
    }

    if (ImGui::CollapsingHeader("Clear"))
    {
        if (ImGui::Button("Clear All"))
        {
            balls.clear();
        }
        if (ImGui::Button("Reset Balls"))
        {
            balls.resetBalls();
        }
    }

    ImGui::End();
}



}
