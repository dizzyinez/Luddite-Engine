#pragma once
#include "Luddite/pch.hpp"
#include "Luddite/Core.hpp"

#include "Luddite/Graphics/Texture.hpp"
#include "Luddite/Platform/DiligentPlatform.hpp"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"
#include "Graphics/GraphicsEngine/interface/SwapChain.h"
#include "Graphics/GraphicsTools/interface/DynamicTextureAtlas.h"

#include "Common/interface/RefCntAutoPtr.hpp"

namespace Luddite
{
class LUDDITE_API PBRRenderer
{
public:
        void Initialize(Diligent::RefCntAutoPtr<Diligent::IRenderDevice> pDevice,
                        Diligent::RefCntAutoPtr<Diligent::IDeviceContext> pImmediateContext,
                        Diligent::RefCntAutoPtr<Diligent::ISwapChain> pSwapChain,
                        Diligent::TEXTURE_FORMAT RTVFormat,
                        // Diligent::TEXTURE_FORMAT DSVFormat,
                        Diligent::RefCntAutoPtr<Diligent::IShaderSourceInputStreamFactory> pShaderSourceFactory,
                        const std::string& VSFilePath,
                        const std::string& PSFilePath);
        inline void SetViewProjMatrix(const glm::mat4& matrix) {m_ViewProjMatrix = matrix;}
        void Draw();
private:
        void CreateRenderPass(Diligent::TEXTURE_FORMAT RTVFormat);
        void CreatePipelineState(Diligent::IShaderSourceInputStreamFactory* pShaderSourceFactory);
        void ReleaseWindowResources();
        Diligent::RefCntAutoPtr<Diligent::IFramebuffer> CreateFramebuffer(Diligent::ITextureView* pDstRenderTarget);
        Diligent::IFramebuffer* GetCurrentFramebuffer();

        // Diligent::RefCntAutoPtr<Diligent::IFramebuffer> m_pFrameBuffer;
        Diligent::RefCntAutoPtr<Diligent::IRenderDevice> m_pDevice;
        Diligent::RefCntAutoPtr<Diligent::IDeviceContext> m_pImmediateContext;
        Diligent::RefCntAutoPtr<Diligent::ISwapChain> m_pSwapChain;

        Diligent::RefCntAutoPtr<Diligent::IRenderPass> m_pRenderPass;
        Diligent::RefCntAutoPtr<Diligent::IBuffer>     m_pShaderConstantsCB;
        glm::mat4 m_ViewProjMatrix;


        #ifdef LD_PLATFORM_DESKTOP
        static constexpr Diligent::TEXTURE_FORMAT DepthBufferFormat = Diligent::TEX_FORMAT_D32_FLOAT;
        #endif

        #ifdef LD_PLATFORM_MOBILE
        static constexpr Diligent::TEXTURE_FORMAT DepthBufferFormat = Diligent::TEX_FORMAT_D16_UNORM;
        #endif

        //TEMP
        Diligent::RefCntAutoPtr<Diligent::IPipelineState>         m_pCubePSO;
        Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> m_pCubeSRB;
        Diligent::RefCntAutoPtr<Diligent::IPipelineState>         m_pAmbientLightPSO;
        Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> m_pAmbientLightSRB;
        Diligent::RefCntAutoPtr<Diligent::ITextureView>           m_CubeTextureSRV;
        Diligent::RefCntAutoPtr<Diligent::IBuffer>                m_CubeVertexBuffer;
        Diligent::RefCntAutoPtr<Diligent::IBuffer>                m_CubeIndexBuffer;

        std::unordered_map<Diligent::ITextureView*, Diligent::RefCntAutoPtr<Diligent::IFramebuffer> > m_FramebufferCache;
        void CreateAmbientLightPSO(Diligent::IShaderSourceInputStreamFactory* pShaderSourceFactory);
};
}