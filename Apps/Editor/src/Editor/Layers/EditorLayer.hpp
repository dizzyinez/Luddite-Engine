#pragma once
#include <Luddite/Luddite.hpp>

#include "Editor/Panels/Viewport.hpp"

class EditorLayer : public Luddite::Layer
{
        public:
        void Initialize();
        void HandleEvents();
        void Update(double delta_time);
        void Render(double alpha, Luddite::RenderTarget window_render_target);
        void RenderImGui(double alpha, Luddite::RenderTarget window_render_target);

        private:
        std::unique_ptr<ViewportPanel> m_pViewportPanel;
};