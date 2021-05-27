#pragma once
#include "Luddite/pch.hpp"
#include "Luddite/Core.hpp"
#include "Luddite/Graphics/DiligentInclude.hpp"

namespace Luddite
{
struct LUDDITE_API Renderer;
struct LUDDITE_API RenderTarget
{
        Diligent::ITextureView* RTV;
        Diligent::ITextureView* DSV;
        unsigned int width = 0;
        unsigned int height = 0;
        bool is_swap_chain_buffer = false;
};
struct LUDDITE_API RenderTexture
{
        inline Diligent::RefCntAutoPtr<Diligent::ITextureView> GetRenderTargetView() {return m_pRTV;}
        inline Diligent::RefCntAutoPtr<Diligent::ITextureView> GetShaderResourceView() {return m_pSRV;}
        inline Diligent::RefCntAutoPtr<Diligent::ITextureView> GetDepthStencilView() {return m_pDSV;}
        inline RenderTarget& GetRenderTarget() {return m_RenderTarget;}
        // Diligent::RefCntAutoPtr<Diligent::ITextureView> GetDepthShaderResourceSRV();

private:
        friend class Renderer;
        RenderTarget m_RenderTarget;
        // Diligent::RefCntAutoPtr<Diligent::IRenderDevice>  m_pDevice;
        // Diligent::RefCntAutoPtr<Diligent::IDeviceContext> m_pImmediateContext;
        Diligent::RefCntAutoPtr<Diligent::ITexture> m_pTexture;
        Diligent::RefCntAutoPtr<Diligent::ITexture> m_pDepthStencil;

        Diligent::RefCntAutoPtr<Diligent::ITextureView> m_pRTV;
        Diligent::RefCntAutoPtr<Diligent::ITextureView> m_pSRV;
        Diligent::RefCntAutoPtr<Diligent::ITextureView> m_pDSV;
        // Diligent::RefCntAutoPtr<Diligent::ITextureView> m_pDepthSRV;
};
}