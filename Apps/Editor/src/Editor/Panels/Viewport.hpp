#pragma once
#include "Editor/Panels/Panel.hpp"
#include "Editor/ECS/Components/Components.hpp"

struct ViewportPanel : public Panel
{
        ViewportPanel()
        {
                SetRenderTexture();
                m_Name = "Viewport";
                m_WindowFlags |= ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse;
                m_Camera.Position = {1.f, 1.f, 1.f};
                m_Camera.ViewDirection = glm::normalize(-m_Camera.Position);
        }

        void Resize()
        {
                if (SetRenderTexture())
                        Luddite::Renderer::ReleaseBufferResources();
        }

        bool SetRenderTexture()
        {
                if (size.x > 0 && size.y > 0)
                {
                        m_RenderTexture = Luddite::Renderer::CreateRenderTexture(size.x, size.y);
                        return true;
                }
                return false;
        }

        void CheckForResize()
        {
                if (m_RenderTexture.GetRenderTarget().width != int(size.x) || m_RenderTexture.GetRenderTarget().height != int(size.y))
                        Resize();
        }


        void DrawImplementation(Luddite::World& world)
        {
                auto max = ImGui::GetWindowContentRegionMax();
                auto min = ImGui::GetWindowContentRegionMin();
                size = glm::ivec2(max.x - min.x, max.y - min.y);
                ImGui::Image(m_RenderTexture.GetShaderResourceView(), ImVec2(size.x, size.y));


                Luddite::EntityID selected_id = world.GetSingleton<C_SelectedEntity>().SelectedEntityID;
                if (selected_id != Luddite::NullEntityID)
                {
                        ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, size.x, size.y);

                        ImGuizmo::SetOrthographic(false);
                        ImGuizmo::SetDrawlist();

                        Luddite::Entity selected = world.GetEntityFromID(selected_id);

                        glm::mat4 projection = m_RenderTexture.GetRenderTarget().GetProjectionMatrix(m_Camera);
                        glm::mat4 view = m_RenderTexture.GetRenderTarget().GetViewMatrix(m_Camera);

                        auto& c_selected_transform = selected.GetComponent<C_Transform3D>();
                        glm::mat4 transform = c_selected_transform.GetTransformMatrix();

                        ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection),
                                ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::LOCAL, glm::value_ptr(transform));
                        if (ImGuizmo::IsUsing())
                        {
                                C_Transform3D replace;
                                ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(transform),
                                        glm::value_ptr(replace.Translation),
                                        glm::value_ptr(replace.Rotation),
                                        glm::value_ptr(replace.Scale)
                                        );
                                selected.ReplaceComponent<C_Transform3D>(replace);
                        }
                }
                if (ImGui::IsMouseHoveringRect(ImVec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y), ImVec2(ImGui::GetWindowPos().x + size.x, ImGui::GetWindowPos().y + size.y)))
                {
                        if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle))
                        {
                                ImVec2 drag = ImGui::GetMouseDragDelta(ImGuiMouseButton_Middle);
                                ImGui::ResetMouseDragDelta(ImGuiMouseButton_Middle);
                                m_CamYaw += drag.x * 0.01;
                                m_CamPitch += drag.y * 0.01;
                                m_CamPitch = glm::clamp(m_CamPitch, -1.57f, 1.57f);
                        }
                }
                m_Camera.Position = glm::vec3(
                        glm::vec4(0.f, 0.f, 5.f, 0.f)
                        * glm::rotate(m_CamPitch, glm::vec3(1.f, 0.f, 0.f))
                        * glm::rotate(m_CamYaw, glm::vec3(0.f, 1.f, 0.f))
                        );
                m_Camera.ViewDirection = glm::normalize(m_CamCenter - m_Camera.Position);
        }

        Luddite::RenderTarget& GetRenderTarget() {return m_RenderTexture.GetRenderTarget();}

        Luddite::Camera m_Camera;
        private:
        Luddite::RenderTexture m_RenderTexture;
        glm::ivec2 size = glm::ivec2(500, 500);

        glm::vec3 m_CamCenter = glm::vec3(0.f);
        float m_CamPitch = 0.f;
        float m_CamYaw = 0.f;
};