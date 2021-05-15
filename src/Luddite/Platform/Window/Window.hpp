#pragma once
#include "Luddite/pch.hpp"
#include "Luddite/Core.hpp"
#include "Luddite/LayerStack.hpp"
#include "Luddite/Rendering/Renderer.hpp"

namespace Luddite
{
class LUDDITE_API Window
{
public:
        Window();
        virtual ~Window();
        virtual void SetTitle(const std::string& title) {};
        virtual void Resize(uint32_t width, uint32_t height) {};
        virtual uint32_t GetWidth() = 0;
        virtual uint32_t GetHeight() = 0;
        std::shared_ptr<LayerStack> GetLayerStack() {return m_layer_stack;}
        Renderer& GetRenderer() {return m_renderer;}
protected:
        std::shared_ptr<LayerStack> m_layer_stack;
        //These functions exists so subclasses of Window can access private members of the Renderer class because friendship doesn't inherit
        Diligent::RefCntAutoPtr<Diligent::IRenderDevice>& GetRenderDevice() {return m_renderer.m_pDevice;}
        Diligent::RefCntAutoPtr<Diligent::IDeviceContext>& GetDeviceContext() {return m_renderer.m_pImmediateContext;}
        Diligent::RefCntAutoPtr<Diligent::ISwapChain>& GetSwapChain() {return m_renderer.m_pSwapChain;}
        Diligent::RefCntAutoPtr<Diligent::IEngineFactory>& GetEngineFactory() {return m_renderer.m_pEngineFactory;}
        // Diligent::RefCntAutoPtr<Diligent::IPipelineState> m_pPSO;
        Renderer m_renderer;
};
}