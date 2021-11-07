#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/Graphics/RenderTarget.hpp"
#include "Luddite/Core/LayerStack.hpp"

namespace Luddite
{
struct IGameInstance
{
        virtual void Initialize() = 0;
        virtual void OnUpdate(float delta_time) = 0;
        virtual void OnFixedUpdate(float delta_time) = 0;
        virtual void OnRender(float lerp_alpha, Luddite::RenderTarget render_target) = 0;
        virtual void OnImGuiRender(float lerp_alpha, Luddite::RenderTarget render_target) = 0;
        virtual Luddite::LayerStack& GetLayerStack() = 0;
        //virtual void LoadWorld(std::shared_ptr<Luddite::Layer> layer, Luddite::World& world) = 0;
};
}
