// #pragma once
#include "agents_app/renderer.h"
#include "agents_app/app.h"
#include "gui_application/glm_to_string.h"

namespace agents_app {

Renderer::Renderer(App& app) 
    : app(app)
{

}

Renderer::~Renderer() 
{

}

void Renderer::setup()
{
    m_drawProgram.setup();
    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);   

}

void Renderer::frame()
{
    m_drawProgram.use();
    m_poseTrace.clear();
    m_poseTrace.push_back(glm::mat4(1));
    glm::mat4 projection_view = app.cameras.projection_view();
    //std::cout << "begin Renderer::frame\n";
    app.root->visit([this,&projection_view](auto& visit, auto* tf) -> void

    {
        if (!tf) return;
        m_poseTrace.push_back(m_poseTrace.back() * tf->transformLocalToParent());
        
        auto drawable = static_cast<Drawable*>(tf->data);
        if (drawable && drawable->enabled())
        {
            glm::mat4 pvm = projection_view * m_poseTrace.back();
            if (drawable->customProgram)
            {
                drawable->draw(pvm);
                m_drawProgram.use();
            }
            else
            {
                if (drawable->instanceOf && drawable->instanceOf->enabled())
                {
                    drawable->instanceOf->drawInstance(pvm);
                }
                else
                {
                    if (drawable->isInstancedRenderer())
                    {
                        //std::cout << "draw '" << drawable->transform.name << "' ; " << std::to_string(visit.depth) << ":" << std::to_string(visit.index) << "\n";
                        m_drawProgram.instanced.set(1);
                        //m_drawProgram.projection_view_model.set(glm::mat4(1));
                    }
                    else
                    {
                        m_drawProgram.instanced.set(0);
                        m_drawProgram.projection_view_model.set(pvm);
                    }
                    drawable->draw();
                }
            }

        }
        if (!drawable || drawable->enabled())
            visit.children();
        else
            visit.skipChildren();
        m_poseTrace.pop_back();
    } );
    //std::cout << "end Renderer::frame\n";

}



}
