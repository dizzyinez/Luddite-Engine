#pragma once
#include "Editor/Panels/Panel.hpp"

struct ViewportPanel : public Panel
{
        ViewportPanel()
        {
                SetRenderTexture();
                m_Name = "Viewport";
                m_WindowFlags |= ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse;
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


        void DrawImplementation()
        {
                auto max = ImGui::GetWindowContentRegionMax();
                auto min = ImGui::GetWindowContentRegionMin();
                size = glm::ivec2(max.x - min.x, max.y - min.y);
                ImGui::Image(
                        m_RenderTexture.GetShaderResourceView(), ImVec2(size.x, size.y)
                        // ImVec2(m_RenderTexture.GetRenderTarget().width,
                        //         m_RenderTexture.GetRenderTarget().height
                        //         )
                        );
        }

        Luddite::RenderTarget GetRenderTarget() {return m_RenderTexture.GetRenderTarget();}

        private:
        Luddite::RenderTexture m_RenderTexture;
        glm::ivec2 size = glm::ivec2(500, 500);
};