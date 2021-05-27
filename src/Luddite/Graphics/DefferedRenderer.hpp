#pragma once
#include "Luddite/pch.hpp"
#include "Luddite/Core.hpp"

#include "Luddite/Graphics/Texture.hpp"
#include "Luddite/Graphics/ShaderAttributeList.hpp"
#include "Luddite/Graphics/DefferedPipelineState.hpp"
#include "Luddite/Graphics/DefferedLightingPipelineState.hpp"
#include "Luddite/Graphics/MaterialLibrary.hpp"
#include "Luddite/Graphics/RenderTarget.hpp"

#include "Luddite/Graphics/DiligentInclude.hpp"

namespace Luddite
{
class LUDDITE_API Renderer;
class LUDDITE_API DefferedRenderer
{
public:
        void Initialize(Diligent::RefCntAutoPtr<Diligent::IRenderDevice> pDevice,
                        Diligent::RefCntAutoPtr<Diligent::IDeviceContext> pImmediateContext,
                        Diligent::TEXTURE_FORMAT ColorBufferFormat,
                        Diligent::RefCntAutoPtr<Diligent::IShaderSourceInputStreamFactory> pShaderSourceFactory
                        );

        void OnWindowResize(int width, int height);
        inline void SetViewProjMatrix(const glm::mat4& matrix) {m_ViewProjMatrix = matrix;}
        void PrepareDraw(RenderTarget& render_target);
        void ApplyLighting();
        void FinalizeDraw();
        DefferedPipelineState BasicShaderPipeline;
        DefferedLightingPipelineState EnvironmentalLightingPipeline;
private:
        friend class Renderer;
        void CreateRenderPass(Diligent::TEXTURE_FORMAT RTVFormat);
        Diligent::RefCntAutoPtr<Diligent::IFramebuffer> CreateFramebuffer();
        void ReleaseWindowResources();
        Diligent::IFramebuffer* GetCurrentFramebuffer();

        RenderTarget* m_pCurrentRenderTarget;

        Diligent::RefCntAutoPtr<Diligent::IRenderDevice> m_pDevice;
        Diligent::RefCntAutoPtr<Diligent::IDeviceContext> m_pImmediateContext;
        Diligent::RefCntAutoPtr<Diligent::IShaderSourceInputStreamFactory> m_pShaderSourceFactory;
        Diligent::IFramebuffer* m_pDrawPeriodFrameBuffer;

        Diligent::RefCntAutoPtr<Diligent::IRenderPass> m_pRenderPass;
        glm::mat4 m_ViewProjMatrix;
        std::unordered_map<Diligent::ITextureView*, Diligent::RefCntAutoPtr<Diligent::IFramebuffer> > m_FramebufferCache;

        #ifdef LD_PLATFORM_DESKTOP
        static constexpr Diligent::TEXTURE_FORMAT DepthBufferFormat = Diligent::TEX_FORMAT_D32_FLOAT;
        #endif

        #ifdef LD_PLATFORM_MOBILE
        static constexpr Diligent::TEXTURE_FORMAT DepthBufferFormat = Diligent::TEX_FORMAT_D16_UNORM;
        #endif

        //Temp:
        Diligent::RefCntAutoPtr<Diligent::IPipelineState>         m_pAmbientLightPSO;
        Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> m_pAmbientLightSRB;
        void CreateAmbientLightPSO(Diligent::IShaderSourceInputStreamFactory* pShaderSourceFactory);
};
}