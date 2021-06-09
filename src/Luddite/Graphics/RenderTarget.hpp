#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/Core/Core.hpp"
#include "Luddite/Graphics/DiligentInclude.hpp"
#include "Luddite/Graphics/Camera.hpp"

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
        Diligent::SURFACE_TRANSFORM PreTransform = Diligent::SURFACE_TRANSFORM_IDENTITY;

        glm::mat4 GetProjectionMatrix(const Camera& camera) const;
        glm::mat4 GetViewMatrix(const Camera& camera) const;
};
struct LUDDITE_API RenderTexture
{
        inline Diligent::RefCntAutoPtr<Diligent::ITextureView> GetRenderTargetView() {return m_pRTV;}
        inline Diligent::RefCntAutoPtr<Diligent::ITextureView> GetShaderResourceView() {return m_pSRV;}
        inline Diligent::RefCntAutoPtr<Diligent::ITextureView> GetDepthStencilView() {return m_pDSV;}
        inline RenderTarget& GetRenderTarget() {return m_RenderTarget;}
        // RenderTexture& operator=(const RenderTexture& other)
        // {
        //         // m_pRTV.Release();
        //         m_pRTV = other.m_pRTV;
        //         m_pSRV = other.m_pSRV;
        //         m_pDSV = other.m_pDSV;
        //         return *this;
        // }
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