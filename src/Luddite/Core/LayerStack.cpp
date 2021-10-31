#include "Luddite/Core/LayerStack.hpp"
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
void LayerStack::UpdateLayersFixed(double delta_time)
{
        UpdateStack();
        //handle events backwards so layers on top can clear events before lower layers
        std::for_each(std::rbegin(m_stack), std::rend(m_stack), [](auto layer) {
                        layer->HandleEvents();
                });

        for (auto layer : m_stack)
                layer->FixedUpdate(delta_time);
}
void LayerStack::RenderLayers(double alpha, Luddite::RenderTarget window_render_target)
{
        for (auto layer : m_stack)
                layer->Render(alpha, window_render_target);
}
void LayerStack::RenderLayersImGui(double alpha, Luddite::RenderTarget window_render_target)
{
        for (auto layer : m_stack)
                layer->RenderImGui(alpha, window_render_target);
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
        if (!layer->initialized)
        {
                layer->Initialize();
                layer->initialized = true;
        }
}
void LayerStack::DefferedPopLayer(std::shared_ptr<Layer> layer)
{
        auto it = std::find(m_stack.begin(), m_stack.end(), layer);
        if (it != m_stack.end())
                m_stack.erase(it);
}


std::vector<std::string> LayerStack::GetLayerNames()
{
        std::vector<std::string> names(m_stack.size());
        for (auto layer : m_stack)
                names.push_back(layer->GetName());
        return names;
}

std::shared_ptr<Layer> LayerStack::GetLayerByName(const std::string& name)
{
        for (auto layer : m_stack)
                if (strcmp(layer->GetName(), name.c_str()) == 0)
                        return layer;
        return nullptr;
}
}
