#include "Luddite/Layers/Editor/ViewportPanel.hpp"
#include "Luddite/Layers/EditorLayer.hpp"
#include "Luddite/Graphics/Renderer.hpp"
#include "Luddite/ECS/Modules/Graphics.hpp"
#include "Luddite/ECS/Modules/Editor.hpp"

Luddite::ViewportPanel::ViewportPanel()
{
        SetRenderTexture();
}
void Luddite::ViewportPanel::Resize()
{
        if (SetRenderTexture())
                Luddite::Renderer::ReleaseBufferResources();
}

bool Luddite::ViewportPanel::SetRenderTexture()
{
        if (size.x > 0 && size.y > 0)
        {
                m_RenderTexture = Luddite::Renderer::CreateRenderTexture(size.x, size.y);
                return true;
        }
        return false;
}

void Luddite::ViewportPanel::CheckForResize()
{
        auto&& rt = m_RenderTexture.GetRenderTarget();
        if (rt.width != (int)size.x || rt.height != (int)size.y)
                Resize();
}

void Luddite::ViewportPanel::OnDraw(EditorContext& ctx)
{
        if (!open)
                return;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
        if (ImGui::Begin("Viewport", &open, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse /*| ImGuiWindowFlags_MenuBar*/))
        {
                //Get the size of the window
                auto window_region_max = ImGui::GetWindowContentRegionMax();
                auto window_region_min = ImGui::GetWindowContentRegionMin();
                size = glm::ivec2(window_region_max.x - window_region_min.x, window_region_max.y - window_region_min.y);
                ImVec2 window_size = ImVec2(size.x, size.y);
                auto window_pos = ImGui::GetWindowPos();
                //Render the image
                ImGui::Image(m_RenderTexture.GetShaderResourceView(), ImVec2(size.x, size.y));
                //Gizmo
                if (ctx.selection.size() > 0)
                {
                        auto& selected_entity = ctx.selection.front();
                        glm::mat4 selection_transform = glm::identity<glm::mat4>();
                        bool has_translation, has_rotation, has_scale = false;
                        if (selected_entity.has<Transform3D::Translation>())
                        {
                                has_translation = true;
                                selection_transform *= selected_entity.get<Transform3D::Translation>()->GetMatrix();
                        }
                        if (selected_entity.has<Transform3D::Rotation>())
                        {
                                has_rotation = true;
                                selection_transform *= selected_entity.get<Transform3D::Rotation>()->GetMatrix();
                        }
                        if (selected_entity.has<Transform3D::Scale>())
                        {
                                has_scale = true;
                                selection_transform *= selected_entity.get<Transform3D::Scale>()->GetMatrix();
                        }

                        if (has_translation || has_rotation || has_scale)
                        {
                                //ImGuizmo::SetRect(window_region_min.x, window_region_min.y, size.x, size.y);
                                ImGuizmo::SetRect(window_pos.x, window_pos.y, size.x, size.y);
                                ImGuizmo::SetOrthographic(camera.Projection == Camera::ProjectionType::ORTHOGRAPHIC);
                                ImGuizmo::SetDrawlist();

                                glm::mat4 projection = m_RenderTexture.GetRenderTarget().GetProjectionMatrix(camera);
                                glm::mat4 view = m_RenderTexture.GetRenderTarget().GetViewMatrix(camera);
                                ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection), ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::LOCAL, glm::value_ptr(selection_transform));
                                //selected_entity.set<Transform3D::TransformMatrix>({selection_transform});
                                if (ImGuizmo::IsUsing())
                                {
                                        glm::vec3 t, r, s;
                                        ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(selection_transform), &t[0], &r[0], &s[0]);
                                        r = glm::radians(r);
                                        if (has_translation)
                                                selected_entity.set<Transform3D::Translation>({t});
                                        if (has_rotation)
                                                selected_entity.set<Transform3D::Rotation>({r});
                                        if (has_scale)
                                                selected_entity.set<Transform3D::Scale>({s});
                                }
                        }
                }
                if (ImGui::IsMouseHoveringRect(window_pos, window_pos + window_size))
                {
                        auto im_drag = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
                        ImGui::ResetMouseDragDelta(ImGuiMouseButton_Right);
                        ctx.world->query<const Editor::CameraParent, Transform3D::Translation, Transform3D::Rotation, Transform3D::Scale>()
                        .iter([ = ](flecs::iter it){
                                auto t = it.term<Transform3D::Translation>(2);
                                auto r = it.term<Transform3D::Rotation>(3);
                                auto s = it.term<Transform3D::Scale>(4);
                                for (auto i : it)
                                {
                                        r[i].Rotation.z += im_drag.x * -0.004;
                                        r[i].Rotation.x += im_drag.y * -0.004;
                                        r[i].Rotation.x = glm::clamp(r[i].Rotation.x, -1.57f, 1.57f);
                                        s[i].Scale.y *= glm::pow(0.85f, ImGui::GetIO().MouseWheel);
                                        s[i].Scale.y = glm::clamp(s[i].Scale.y, 0.01f, 500.f);
                                }
                        });
                }
        }
        ImGui::End();
        ImGui::PopStyleVar();
}
void Luddite::ViewportPanel::OnRender(EditorContext& ctx)
{
        if (!open)
                return;
        CheckForResize();
        ctx.world->set<Graphics::MainWindow>({m_RenderTexture.GetRenderTarget()});
        ecs_set_pipeline(*ctx.world, ctx.world->id<Luddite::RenderPipeline>());
        ctx.world->progress();
        ctx.world->query<const Graphics::Camera, const Graphics::RenderToMainWindow, const Transform3D::Translation, const Transform3D::Rotation>().iter([&](flecs::iter it){
                auto c = it.term<const Graphics::Camera>(1);
                auto t = it.term<const Transform3D::Translation>(3);
                auto r = it.term<const Transform3D::Rotation>(4);
                //just do first entity
                int i = 0;
                camera =
                {
                        t[i].Translation,
                        r[i].GetLookDirection(),
                        c[i].Projection,
                        c[i].FOV,
                        c[i].OrthoScale,
                        c[i].ClipNear,
                        c[i].ClipFar
                };
        });
}
