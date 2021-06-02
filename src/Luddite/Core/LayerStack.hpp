#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/Core/Core.hpp"

namespace Luddite
{
class LUDDITE_API LayerStack;
class LUDDITE_API Layer
{
public:
        virtual ~Layer() {}
protected:
        friend class LayerStack;
        virtual void Initialize() {}
        virtual void HandleEvents() {}
        virtual void Update(double delta_time) {}
        virtual void Render(double alpha) {}
        entt::registry m_Registry;
};
class LUDDITE_API LayerStack
{
public:
        void UpdateLayers(double delta_time);
        void RenderLayers(double alpha);
        void PushLayer(std::shared_ptr<Layer> layer);
        void PopLayer(std::shared_ptr<Layer> layer);
private:
        void UpdateStack();
        void DefferedPushLayer(std::shared_ptr<Layer> layer);
        void DefferedPopLayer(std::shared_ptr<Layer> layer);
        std::vector<std::shared_ptr<Layer> > m_stack{};
        std::vector<std::shared_ptr<Layer> > m_remove_queue{};
        std::vector<std::shared_ptr<Layer> > m_add_queue{};
};
}