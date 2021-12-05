#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/Core/LayerStack.hpp"
#include "Luddite/Layers/Editor/Panel.hpp"

namespace Luddite
{
struct EditorContext
{
        flecs::world* world;
        std::vector<flecs::entity> selection;
};
class LUDDITE_API EditorLayer : public Layer
{
        protected:
        virtual void Initialize() override;
        virtual void HandleEvents() override;
        virtual void Update(double delta_time) override;
        virtual void FixedUpdate(double delta_time) override;
        virtual void Render(double alpha, RenderTarget render_target) override;
        virtual void RenderImGui(double alpha, RenderTarget render_target) override;
        virtual const char* GetName() override {return "Editor";}
        std::vector<std::shared_ptr<Panel> > m_Panels;
        EditorContext m_Ctx;
        private:
        inline void AddPanel(std::shared_ptr<Panel> panel) {m_Panels.emplace_back(panel);}
        template <typename P>
        inline void AddPanel() {m_Panels.emplace_back(std::make_shared<P>());}
        void RenderDockSpace();
};
}
