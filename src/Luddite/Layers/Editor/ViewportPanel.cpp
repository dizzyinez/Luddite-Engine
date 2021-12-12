#include "Luddite/Layers/Editor/ViewportPanel.hpp"
#include "Luddite/Layers/EditorLayer.hpp"
#include "Luddite/Graphics/Renderer.hpp"
#include "Luddite/ECS/Modules/Graphics.hpp"

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
                {
                        auto max = ImGui::GetWindowContentRegionMax();
                        auto min = ImGui::GetWindowContentRegionMin();
                        size = glm::ivec2(max.x - min.x, max.y - min.y);
                }
                //Render the image
                ImGui::Image(m_RenderTexture.GetShaderResourceView(), ImVec2(size.x, size.y));
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
}
