#include "Editor/Layers/EditorLayer.hpp"
#include "Editor/ECS/Components/Components.hpp"

Luddite::Entity cam;
void EditorLayer::Initialize()
{
        m_World.RegisterSystem<S_SceneSubmitter>();
        m_World.RegisterSystem<S_RenderActiveCamera>();
        m_World.ConfigureSystems();
        m_pViewportPanel = std::make_unique<ViewportPanel>();

        {
                auto e = m_World.CreateEntity();
                auto& transform = e.AddComponent<C_Transform3D>();
                // transform.Translation.x = 1.f;
                // transform.Translation.y = 1.f;
                // transform.Translation.z = 1.f;
                e.AddComponent<C_Model>(Luddite::ModelLoader::GetBasicModel("Assets/suzanne.obj"));
                m_World.SetSingleton<C_SelectedEntity>(e.GetID());
        }

        {
                cam = m_World.CreateEntity();
                auto& transform = cam.AddComponent<C_Transform3D>();
                cam.AddComponent<C_Camera>();
                m_World.SetSingleton<C_ActiveCamera>(cam.GetID());
        }
}

void EditorLayer::HandleEvents()
{
}

float yaw = 0.f;
float pitch = 0.f;

void EditorLayer::Update(double delta_time)
{
        C_Transform3D transform = cam.GetComponent<C_Transform3D>();
        transform.Translation = glm::vec3((glm::rotate(yaw, glm::vec3(0.f, 1.f, 0.f)) * glm::rotate(pitch, glm::vec3(1.f, 0.f, 0.f)))
                * glm::vec4(0.f, 0.f, -3.f, 1.f));
        cam.ReplaceComponent<C_Transform3D>(transform);
        LD_LOG_TRACE("{}", glm::to_string(transform.Translation));
        // cam.ReplaceComponent<C_Transform3D>();
}

void EditorLayer::Render(double alpha, Luddite::RenderTarget window_render_target)
{
        m_World.UpdateSystem<S_SceneSubmitter>(m_World, alpha);
        m_pViewportPanel->CheckForResize();
        Luddite::Renderer::Draw(m_pViewportPanel->GetRenderTarget(), m_pViewportPanel->m_Camera);
        // m_World.UpdateSystem<S_RenderActiveCamera>(m_World, alpha, m_pViewportPanel->GetRenderTarget());
}

void EditorLayer::RenderImGui(double alpha, Luddite::RenderTarget window_render_target)
{
        // m_World.UpdateSystem<S_Render>(m_World, alpha, window_render_target);


        {
                static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
                ImGuiViewport* viewport = ImGui::GetMainViewport();
                ImGui::SetNextWindowPos(viewport->Pos);
                ImGui::SetNextWindowSize(viewport->Size);
                ImGui::SetNextWindowViewport(viewport->ID);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
                ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
                window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
                window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
                ImGui::Begin("DockSpace", nullptr, window_flags);
                ImGui::PopStyleVar();
                ImGui::PopStyleVar(2);
                // DockSpace
                ImGuiIO& io = ImGui::GetIO();
                if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
                {
                        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
                        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

                        static auto first_time = true;
                        if (first_time)
                        {
                                first_time = false;

                                ImGui::DockBuilderRemoveNode(dockspace_id); // clear any previous layout
                                ImGui::DockBuilderAddNode(dockspace_id, dockspace_flags | ImGuiDockNodeFlags_DockSpace);
                                ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

                                // split the dockspace into 2 nodes -- DockBuilderSplitNode takes in the following args in the following order
                                //   window ID to split, direction, fraction (between 0 and 1), the final two setting let's us choose which id we want (which ever one we DON'T set as NULL, will be returned by the function)
                                //                                                              out_id_at_dir is the id of the node in the direction we specified earlier, out_id_at_opposite_dir is in the opposite direction

                                // auto dock_id_center = ImGui::DockBuilderAddNode(dockspace_id);
                                // auto dock_id_right = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_None, 0.2f, nullptr, &dockspace_id);
                                auto dock_id_left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.2f, nullptr, &dockspace_id);
                                auto dock_id_down = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 0.25f, nullptr, &dockspace_id);

                                // we now dock our windows into the docking node we made above
                                ImGui::DockBuilderDockWindow("Viewport", dockspace_id);
                                ImGui::DockBuilderDockWindow("Down", dock_id_down);
                                ImGui::DockBuilderDockWindow("Left", dock_id_left);
                                ImGui::DockBuilderFinish(dockspace_id);
                        }
                }
                ImGui::End();

                ImGui::Begin("Left");
                ImGui::Text("Hello, left!");
                ImGui::SliderAngle("Pitch", &pitch);
                ImGui::SliderAngle("Yaw", &yaw);
                ImGui::End();

                ImGui::Begin("Down");
                ImGui::Text("Hello, down!");
                ImGui::End();
        }

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        m_pViewportPanel->Draw(m_World);
        ImGui::PopStyleVar();
}