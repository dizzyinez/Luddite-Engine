#include "LayerStack.hpp"
namespace Luddite
{
void LayerStack::UpdateLayers(double delta_time)
{
        UpdateStack();
        //handle events backwards so layers on top can clear events before lower layers
        std::for_each(std::rbegin(m_stack), std::rend(m_stack), [](auto layer) {
                        layer->HandleEvents();
                });

        for (auto layer : m_stack)
                layer->Update(delta_time);
}
void LayerStack::RenderLayers(double alpha)
{
        for (auto layer : m_stack)
                layer->Render(alpha);
}
void LayerStack::PushLayer(std::shared_ptr<Layer> layer)
{
        m_add_queue.push_back(layer);
}
void LayerStack::PopLayer(std::shared_ptr<Layer> layer)
{
        m_remove_queue.push_back(layer);
}
void LayerStack::UpdateStack()
{
        for (auto layer : m_add_queue)
                DefferedPushLayer(layer);
        for (auto layer : m_remove_queue)
                DefferedPopLayer(layer);
        m_add_queue.clear();
        m_remove_queue.clear();
}

void LayerStack::DefferedPushLayer(std::shared_ptr<Layer> layer)
{
        m_stack.emplace_back(layer);
        layer->Initialize();
}
void LayerStack::DefferedPopLayer(std::shared_ptr<Layer> layer)
{
        auto it = std::find(m_stack.begin(), m_stack.end(), layer);
        if (it != m_stack.end())
                m_stack.erase(it);
}
// std::vector<std::shared_ptr<Layer> > m_stack;
// std::vector<std::shared_ptr<Layer> > m_remove_queue;
// std::vector<std::shared_ptr<Layer> > m_add_queue;
}