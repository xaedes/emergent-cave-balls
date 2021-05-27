#pragma once

#include "gui_application/version.h"
#include "gui_application/gui_application.h"
#include "gui_application/cameras/cameras.h"
#include "gui_application/drawing/cube.h"
#include "gui_application/drawing/lines.h"
#include "gui_application/drawing/cube_instanced.h"
#include "gui_application/shader/default_program.h"
// #include "gui_application/transform/transform.h"
#include "gui_application/glm_to_string.h"
#include "transform/transform.h"
#include "csv_row/csv_row.h"

#include "imgui.h"

#include <random>
#include <chrono>
#include <algorithm>

namespace gui_application {

    class CubeDemo : public GuiApplication
    {
    public:
        using Transform = drawing::Drawable::Transform;
        CubeDemo() : GuiApplication(true) {}
        virtual ~CubeDemo() {}
        void setup() override
        {
            // glfwSwapInterval(0);
            m_activeCamera = 1;
            m_orbitalCamera.view.distance = 36;
            m_drawProgram.setup();
            m_tfRoot = m_pool.create<Transform>("root");
            m_tfGroup = m_pool.create<Transform>("group", m_tfRoot);
            m_tfGroup2 = m_pool.create<Transform>("group2", m_tfRoot);
            m_lines = m_pool.create<drawing::Lines>(m_pool, "lines", m_tfRoot);
            m_framesContainer = m_pool.create<drawing::Lines>(m_pool, "frames", *m_lines);
            m_cubeInstanced = m_pool.create<drawing::CubeInstanced>(m_pool, "cube_instanced", m_tfRoot);
            //m_cube = drawing::Drawable::Make<drawing::Cube>(m_entityManager.memoryPool, "cube", m_tfRoot, glm::scale(glm::vec3(5, 5, 5)));
            //m_cube2 = drawing::Drawable::Make<drawing::Cube>(m_entityManager.memoryPool, "cube 2", m_tfGroup, glm::translate(glm::vec3(5, 5, 10)));
            //m_cube3 = drawing::Drawable::Make<drawing::Cube>(m_entityManager.memoryPool, "cube 3", m_tfGroup, glm::translate(glm::vec3(10,10,10)));

            auto cubeInstanced = m_cubeInstanced->as<drawing::CubeInstanced>();
            std::random_device rd;  //Will be used to obtain a seed for the random number engine
            //rnd = std::mt19937(rd()); //Standard mersenne_twister_engine seeded with rd()
            rnd = std::mt19937(1337); //Standard mersenne_twister_engine seeded with rd()
            std::uniform_real_distribution<> distrib(-10,10);
            m_manyCubes.clear();
            m_cubeTargets.clear();
            while (m_manyCubes.size() < 10)
            //while (m_manyCubes.size() < 100)
            // while (m_manyCubes.size() < 50000)
            {
                m_manyCubes.push_back(
                    cubeInstanced->Cube(
                        m_pool,
                        "another cube", 
                        m_tfGroup2, 
                        glm::scale(
                            glm::translate(
                                glm::vec3(
                                    distrib(rnd), 
                                    distrib(rnd), 
                                    distrib(rnd)
                                )
                            ),
                            glm::vec3(0.05, 0.05, 0.05)
                        )
                    )
                );
                //std::cout << "added " << (m_manyCubes.size()-1) << "   " << m_manyCubes.back()->transform << "\n";
                m_cubeTargets.push_back(glm::vec3(
                    distrib(rnd),
                    distrib(rnd),
                    distrib(rnd)
                ));
                // m_manyCubes.push_back(
                //     drawing::Drawable::Make<drawing::Cube>(
                //         "another cube", 
                //         m_tfGroup2, 
                //         glm::translate(
                //             glm::vec3(
                //                 distrib(gen), 
                //                 distrib(gen), 
                //                 distrib(gen)
                //             )
                //         )
                //     )
                // );

            }
            //m_cube->setup();
            //m_cube2->setup();
            //m_cube3->setup();
            for (auto it = m_tfRoot->begin_recurse_data<drawing::Drawable>(); it != m_tfRoot->end_recurse_data<drawing::Drawable>(); ++it)
            {
                drawing::Drawable* drawable = static_cast<drawing::Drawable*>(it);
                if(drawable) drawable->setup();
            }
            //m_tfRoot->visit([this](auto& visitor, const drawing::Drawable::Transform::pointer& tf)
            //{
                //if (!tf) return;
                //if (tf->object) tf->object->setup();
            //});

            auto lines = m_framesContainer->as<drawing::Lines>();
            lines->drawLine(
                glm::vec3(0,0,0),
                glm::vec3(10,0,0),
                glm::vec3(1,0,0),
                glm::vec3(1,0,0)
            );            
            lines->drawLine(
                glm::vec3(0,0,0),
                glm::vec3(0,10,0),
                glm::vec3(0,1,0),
                glm::vec3(0,1,0)
            );            
            lines->drawLine(
                glm::vec3(0,0,0),
                glm::vec3(0,0,10),
                glm::vec3(0,0,1),
                glm::vec3(0,0,1)
            );

            std::ifstream theFile("D:/iff/rave/ccobjects.csv");
            std::cout << "theFile.good() " << theFile.good() << std::endl;
            csv_row::CsvRow row;
            int lastFrameNum = 0;
            drawing::Lines* currentFrame = nullptr;
            while(theFile >> row)
            {   
                int k=6;
                int frameNum = std::stoi(row[1]);
                if (currentFrame == nullptr || frameNum > lastFrameNum)
                {
                    auto* ptr = m_pool.create<drawing::Lines>(m_pool, "", *m_framesContainer);
                    m_frames.push_back(ptr);
                    currentFrame = ptr->as<drawing::Lines>();
                    currentFrame->setup();
                }
                lastFrameNum = frameNum;
                if (!currentFrame) break;
                float time_scale = 1.0f;
                float rcolor = (255 - abs(((frameNum) % (2 * 255)) - 255)) / 255.0f;
                float gcolor = (255 - abs(((frameNum+128) % (2 * 255)) - 255)) / 255.0f;
                float bcolor = (255 - abs(((frameNum+255) % (2 * 255)) - 255)) / 255.0f;
                float saturation = 1.5;
                rcolor = (rcolor - 0.5) * saturation + 0.5;
                gcolor = (gcolor - 0.5) * saturation + 0.5;
                bcolor = (bcolor - 0.5) * saturation + 0.5;
                if (rcolor < 0) rcolor = 0; if (rcolor > 1) rcolor = 1;
                if (gcolor < 0) gcolor = 0; if (gcolor > 1) gcolor = 1;
                if (bcolor < 0) bcolor = 0; if (bcolor > 1) bcolor = 1;
                glm::vec3 color(rcolor, gcolor, bcolor);
                currentFrame->transform.setLocalPosition(glm::vec3(0,time_scale*frameNum,0));
                currentFrame->drawLine(
                    glm::vec3(std::stod(row[6]), 0, std::stod(row[7])),
                    glm::vec3(std::stod(row[8]), 0, std::stod(row[9])),
                    color,
                    color
                );
                currentFrame->drawLine(
                    glm::vec3(std::stod(row[8]), 0, std::stod(row[9])),
                    glm::vec3(std::stod(row[10]), 0, std::stod(row[11])),
                    color,
                    color
                );
                currentFrame->drawLine(
                    glm::vec3(std::stod(row[10]), 0, std::stod(row[11])),
                    glm::vec3(std::stod(row[12]), 0, std::stod(row[13])),
                    color,
                    color
                );
                currentFrame->drawLine(
                    glm::vec3(std::stod(row[12]), 0, std::stod(row[13])),
                    glm::vec3(std::stod(row[6]), 0, std::stod(row[7])),
                    color,
                    color
                );
                //std::cout << "row.size() " << row.size() << "\n";
                //break;
            }
            std::cout << "m_frames.size() " << m_frames.size() << "\n";
            m_orbitalCamera.view.angles.y = -1.65;
            m_orbitalCamera.view.angles.z = 1.57;
            m_orbitalCamera.view.distance = 100;
            // Enable depth test
            glEnable(GL_DEPTH_TEST);
            // Accept fragment if it closer to the camera than the former one
            glDepthFunc(GL_LESS);            
        }

        void imgui_transform(const Transform::pointer& tf)
        {
            auto localPos = tf->localPosition();
            auto scale = tf->localScale();
            auto eulerXYZ = glm::degrees( tf->localRotationEulerXYZ() );
            auto strWorldPose = std::to_string(tf->worldPose());

            ImGui::Text(strWorldPose.c_str());

            bool updatedPosition = false;
            updatedPosition |= ImGui::SliderFloat("localPos.x", &localPos.x, -100, 100);
            updatedPosition |= ImGui::SliderFloat("localPos.y", &localPos.y, -100, 100);
            updatedPosition |= ImGui::SliderFloat("localPos.z", &localPos.z, -100, 100);

            bool updatedScale = false;
            updatedScale |= ImGui::SliderFloat("scale.x", &scale.x, 0, 10);
            updatedScale |= ImGui::SliderFloat("scale.y", &scale.y, 0, 10);
            updatedScale |= ImGui::SliderFloat("scale.z", &scale.z, 0, 10);

            bool updatedEulerXYZ = false;
            updatedEulerXYZ |= ImGui::SliderFloat("euler.x", &eulerXYZ.x, -360, 360);
            updatedEulerXYZ |= ImGui::SliderFloat("euler.y", &eulerXYZ.y, -(90-1e-3), (90 - 1e-3));
            updatedEulerXYZ |= ImGui::SliderFloat("euler.z", &eulerXYZ.z, -360, 360);
            if (updatedPosition)
            {
                tf->setLocalPosition(localPos);
            }
            if (updatedScale)
            {
                tf->setLocalScale(scale);
            }
            if (updatedEulerXYZ)
            {
                tf->setLocalRotationEulerXYZ(glm::radians(eulerXYZ));
            }
        }
        void update()
        {
            auto now = std::chrono::high_resolution_clock::now();
            double dt = std::chrono::duration<double>(now - m_lastTime).count();
            if (dt > 1) dt = 1.0 / 30.0;

            static float activeFrame = 0;
            static float frameAutoplaySpeed = 0;
            static int framesContextMax = m_frames.size() - 1;
            static int framesContext = 100;
            static bool roundFrameOffset = false;
            ImGui::Begin("Frames");
            ImGui::SliderFloat("activeFrame", &activeFrame, 0, m_frames.size()-1);
            ImGui::SliderFloat("frameAutoplaySpeed", &frameAutoplaySpeed, 0, 180);
            ImGui::Checkbox("roundFrameOffset", &roundFrameOffset);
            ImGui::SliderInt("framesContextMax", &framesContextMax, 0, m_frames.size()-1);
            ImGui::SliderInt("framesContext", &framesContext, 1, framesContextMax);
            ImGui::End();
            activeFrame += frameAutoplaySpeed * dt;
            while (activeFrame > m_frames.size()) activeFrame -= m_frames.size();
            
            m_framesContainer->transform.setLocalPosition(glm::vec3(0, -(roundFrameOffset ? (int)activeFrame : activeFrame), 0));
            for (int i = 0; i < m_frames.size(); ++i)
            {
                m_frames[i]->enabled() = (abs(i - activeFrame) <= framesContext);
            }

            std::uniform_real_distribution<> unitinterval(0,1);
            std::uniform_real_distribution<> distrib(-10, 10);
            static int maxCubes = m_manyCubes.size();
            static float probDelete = 0*0.05;
            static float probSpawn = 0*0.05;
            static float cubeScale = 0.05;
            static float rotSpeedX = 0;
            static float rotSpeedY = 0;
            static float rotSpeedZ = 0;
            ImGui::Begin("Update");
            ImGui::SliderInt("Maximum Count", &maxCubes, 0, 50000);
            ImGui::SliderFloat("Probability to delete", &probDelete, 0, 1);
            ImGui::SliderFloat("Probability to spawn", &probSpawn, 0, 1);
            ImGui::SliderFloat("Scale", &cubeScale, 0, 1);
            ImGui::SliderFloat("Rotation Speed X", &rotSpeedX, 0, 1);
            ImGui::SliderFloat("Rotation Speed Y", &rotSpeedY, 0, 1);
            ImGui::SliderFloat("Rotation Speed Z", &rotSpeedZ, 0, 1);
            ImGui::End();
            static int counter = 0;
            static std::vector<int> countDelete;
            static std::vector<int> countSpawned;
            if (countSpawned.size() == 0)
            {
                countSpawned.resize(m_manyCubes.size());
                std::fill_n(countSpawned.begin(), m_manyCubes.size(), 1);
            }
            if (maxCubes > m_manyCubes.size())
            {
                auto oldSize = m_manyCubes.size();
                m_manyCubes.resize(maxCubes);

                m_cubeTargets.resize(m_manyCubes.size());
                for (int i = oldSize; i < m_manyCubes.size(); ++i)
                {
                    m_manyCubes[i] = nullptr;
                }

            }
            else if (maxCubes < m_manyCubes.size())
            {
                for (int i = maxCubes; i < m_manyCubes.size(); ++i)
                {
                    if (!m_manyCubes[i]) continue;
                    //bool res = m_pool.destroy(m_manyCubes[i]->transform);
                    //assert(res == true);
                    m_pool.destroy(m_manyCubes[i]);
                    m_manyCubes[i] = nullptr;
                }
                m_manyCubes.resize(maxCubes);
            }

            countDelete.resize(m_manyCubes.size());
            countSpawned.resize(m_manyCubes.size());

            counter++;
            for (int i = 0; i < m_manyCubes.size(); ++i)
            {
                if (!m_manyCubes[i])
                {
                    auto cubeInstanced = static_cast<drawing::CubeInstanced*>(m_cubeInstanced);
                    if (unitinterval(rnd) <= probSpawn)
                    {
                        ++countSpawned[i];
                        m_manyCubes[i] = (
                            cubeInstanced->Cube(
                                m_pool,
                                "another cube",
                                m_tfGroup2,
                                glm::scale(
                                    glm::translate(
                                        glm::vec3(
                                            distrib(rnd),
                                            distrib(rnd),
                                            distrib(rnd)
                                        )
                                    ),
                                    glm::vec3(cubeScale, cubeScale, cubeScale)
                                )
                            )
                        );
                        m_manyCubes[i]->setup();
                        //std::cout << counter << "\t" << "added " << i << "   " << m_manyCubes[i]->transform << "\n";
                        m_cubeTargets[i] = (glm::vec3(
                            distrib(rnd),
                            distrib(rnd),
                            distrib(rnd)
                        ));
                    }
                    continue;
                };
                if (unitinterval(rnd) <= probDelete)
                {
                    //std::cout << counter << "\t" << "remove " << i << "   " << m_manyCubes[i]->transform << " ";
                    ++countDelete[i];
                    //bool res = m_pool.destroy(m_manyCubes[i]->transform);
                    //assert(res == true);
                    m_pool.destroy(m_manyCubes[i]);
                    m_manyCubes[i] = nullptr;
                    // m_manyCubes[i].reset();
                    continue;
                }
                if (!m_manyCubes[i]) continue;

                auto pos = m_manyCubes[i]->transform.localPosition();
                auto& target = m_cubeTargets[i];
                auto diff = target - pos;
                auto distance = glm::length(diff);
                if (distance < 1e-3)
                {
                    m_manyCubes[i]->transform.setLocalPosition(target);
                    m_cubeTargets[i] = glm::vec3(
                        distrib(rnd),
                        distrib(rnd),
                        distrib(rnd)
                    );
                }
                else
                {
                    const float maxDist = dt * 1;
                    if (distance > maxDist)
                    {
                        diff = diff * (maxDist / distance);
                    }
                    m_manyCubes[i]->transform.setLocalPosition(pos + diff);
                }
                m_manyCubes[i]->transform.setLocalScale(glm::vec3(cubeScale, cubeScale, cubeScale));
                m_manyCubes[i]->transform.setLocalRotationEulerXYZ(
                    glm::vec3(dt * rotSpeedX, dt * rotSpeedY, dt * rotSpeedZ) + m_manyCubes[i]->transform.localRotationEulerXYZ());
            }

            m_lastTime = now;            
        }
        void frame() override
        {
            int display_w, display_h;

            glfwGetFramebufferSize(m_window, &display_w, &display_h);

            //if (m_showDemoWindow)
            //{
            //}
            bool showMetricsWindow = true;
            bool showDemoWindow = true;
            ImGui::ShowDemoWindow(&showDemoWindow);
            ImGui::ShowMetricsWindow(&showMetricsWindow);

            ImGui::Begin("Transforms");

            // m_tfRoot->foreachRecursive([](int recurseDepth, const Drawable::pointer& obj)
            // m_tfRoot->foreachRecursive([](auto& traversal, int recurseDepth, const Drawable::TransformPtr& tf)
            
            m_tfRoot->visit([this](auto& visit, auto* tf) -> void
            // {}
            // );
             // for (auto it = m_tfRoot->begin_recurse(); it != m_tfRoot->end_recurse(); ++it)
            {
                // cant really cast from Hierarchical to Transform...
                 // auto tf = static_cast<Transform::pointer>(it);
                if (!tf) return;
                //if (!tf) continue;
            //}
            //    m_tfRoot->visitRaw([this](auto& visitor, const drawing::Drawable::Transform::pointer& tf)
            //{
            //    if (!tf) return;
                // std::string name = tf->name;
                std::string name = std::to_string(visit.depth) + " " + std::to_string(visit.index) + " " + tf->name;
                if (ImGui::TreeNode(name.c_str()))
                {
                    //if (ImGui::TreeNode(("transform#" + name).c_str()))
                    if (ImGui::TreeNode("transform"))
                    {
                        imgui_transform(tf);
                        ImGui::TreePop();
                    }
                    //if (ImGui::TreeNode(("children#" + name).c_str()))
                    if (ImGui::TreeNode("children"))
                    {
                        visit.children();
                        ImGui::TreePop();
                    }
                    else visit.skipChildren();
                    ImGui::TreePop();
                }
                else visit.skipChildren();
            }  );
            ImGui::End();

            // ImGui::Begin("Cube Transform");
            // imgui_transform(m_cube.transform);
            // ImGui::End();

            ImGui::Begin("Camera");
            //ImGui::BeginGroup();
            //ImGui::Text("Choose Camera");
            //ImGui::RadioButton("First Person Camera", &m_activeCamera, 0);
            //ImGui::RadioButton("Orbital Camera", &m_activeCamera, 1);
            //ImGui::EndGroup();

            ImGui::BeginTabBar("Camera Options");
            bool firstPersonCameraOpen = m_activeCamera == 0;
            bool orbitalCameraOpen = m_activeCamera == 1;
            bool lookAtCameraOpen = m_activeCamera == 2;
            if (ImGui::BeginTabItem("Orbital Camera"))
            {
                ImGui::SliderFloat("target.x", &m_orbitalCamera.view.target.x, -100, 100);
                ImGui::SliderFloat("target.y", &m_orbitalCamera.view.target.y, -100, 100);
                ImGui::SliderFloat("target.z", &m_orbitalCamera.view.target.z, -100, 100);
                ImGui::SliderFloat("roll", &m_orbitalCamera.view.angles.x, -3.14159 * 2, 3.14159 * 2);
                ImGui::SliderFloat("pitch", &m_orbitalCamera.view.angles.y, -3.14159 * 2, 3.14159 * 2);
                ImGui::SliderFloat("yaw", &m_orbitalCamera.view.angles.z, -3.14159 * 2, 3.14159 * 2);
                ImGui::SliderFloat("distance", &m_orbitalCamera.view.distance, -200, 200);
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

            update();

            glm::mat4 projection_view;
            switch (m_activeCamera)
            {
            case 0:
                m_firstPersonCamera.updateViewport({ display_w, display_h });
                projection_view = m_firstPersonCamera.projection.mat() * m_firstPersonCamera.view.mat();
                break;
            case 1:
                m_orbitalCamera.updateViewport({ display_w, display_h });
                projection_view = m_orbitalCamera.projection.mat() * m_orbitalCamera.view.mat();
                break;
            case 2:
                m_lookAtCamera.updateViewport({ display_w, display_h });
                projection_view = m_lookAtCamera.projection.mat() * m_lookAtCamera.view.mat();
                break;
            }



            // m_tfRoot->visit([this](auto& visitor, const drawing::Drawable::TransformRawPtr& tf)
            // {
            //     std::sort(tf->children().begin(), tf->children().end(), [](const auto& tfA, const auto& tfB) {
            //         auto a = tfA->local
            //         return (
            //             ((a[3][3] == 0) || (b[3][3] == 0))
            //             ? false
            //             : ((a[3][2] / a[3][3]) > (b[3][2] / b[3][3]))
            //         );
            //     });
            // });



            m_drawProgram.use();
            m_poseTrace.clear();
            m_poseTrace.push_back(glm::mat4(1));

            m_tfRoot->visit([this,&projection_view](auto& visit, auto* tf) -> void

            //m_tfRoot->visitRaw([this,&projection_view](auto& visitor, const drawing::Drawable::Transform::pointer& tf)
            //{
            // for (auto it = m_tfRoot->begin_recurse(); it != m_tfRoot->end_recurse(); ++it)
            {
                // auto tf = static_cast<Transform::pointer>(it);
                // if (!tf) continue;
                if (!tf) return;
                m_poseTrace.push_back(m_poseTrace.back() * tf->transformLocalToParent());
                
                //if (tf->children().size())
                //std::sort(tf->children().begin(), tf->children().end(), [&pvm](const auto& tfA, const auto& tfB) {
                //    auto a = pvm * tfA->transformLocalToParent();
                //    auto b = pvm * tfB->transformLocalToParent();
                //    return (
                //        ((a[3][3] == 0) || (b[3][3] == 0))
                //        ? false
                //        : ((a[3][2] / a[3][3]) > (b[3][2] / b[3][3]))
                //    );
                //});
                auto drawable = static_cast<drawing::Drawable*>(tf->data);
                if (drawable && drawable->enabled())
                {
                
                    //if (drawable)
                    //{
                    glm::mat4 pvm = projection_view * m_poseTrace.back();
                    if (drawable->instanceOf)
                    {
                        drawable->instanceOf->drawInstance(pvm);
                    }
                    else
                    {
                        if (drawable->isInstancedRenderer())
                        {
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
                    //}
                    //visit.children();
                }
                //else
                //{
                //    visit.skipChildren();
                //}
                visit.children();
                m_poseTrace.pop_back();
            } );
        }
    protected:
        cameras::FirstPersonPerspectiveCamera m_firstPersonCamera;
        cameras::OrbitalPerspectiveCamera m_orbitalCamera;
        cameras::LookAtPerspectiveCamera m_lookAtCamera;
        
        int m_activeCamera = 0;
        std::vector<glm::mat4> m_poseTrace;
        drawing::Drawable::pointer m_cube;
        drawing::Drawable::pointer m_cube2;
        drawing::Drawable::pointer m_cube3;
        drawing::Drawable::pointer m_cubeInstanced;
        drawing::Drawable::pointer m_lines;
        drawing::Drawable::pointer m_framesContainer;
        std::vector<drawing::Drawable::pointer> m_manyCubes;
        std::vector<drawing::Drawable::pointer> m_frames;
        std::vector<glm::vec3> m_cubeTargets;

        shader::DefaultProgram m_drawProgram;
        bool m_showDemoWindow;

        drawing::Drawable::memory_pool_type m_pool;

        Transform::pointer m_tfRoot;
        Transform::pointer m_tfGroup;
        Transform::pointer m_tfGroup2;

        std::mt19937 rnd;
        std::chrono::time_point<std::chrono::high_resolution_clock> m_lastTime;
    };

} // namespace gui_application
