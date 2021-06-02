#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/Core/Core.hpp"

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
        enum class G_BUFFER_FLAGS : uint8_t
        {
                COLOR = 1,
                NORMAL = 2,
                DEPTH = 4
        };
private:
        struct FrameBufferData
        {
                Diligent::RefCntAutoPtr<Diligent::IFramebuffer> pFrameBuffer;
                Diligent::RefCntAutoPtr<Diligent::ITexture> pOpenGLRenderTexture;
                int SRBIndex;
        };
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
        FrameBufferData* GetCurrentFramebufferData();
        FrameBufferData CreateFramebuffer();
        void ReleaseWindowResources();

        RenderTarget m_pCurrentRenderTarget;

        Diligent::RefCntAutoPtr<Diligent::IRenderDevice> m_pDevice;
        Diligent::RefCntAutoPtr<Diligent::IDeviceContext> m_pImmediateContext;
        Diligent::RefCntAutoPtr<Diligent::IShaderSourceInputStreamFactory> m_pShaderSourceFactory;
        FrameBufferData* m_DrawPeriodFrameBufferData;

        Diligent::RefCntAutoPtr<Diligent::IRenderPass> m_pRenderPass;
        glm::mat4 m_ViewProjMatrix;
        std::unordered_map<Diligent::ITextureView*, FrameBufferData> m_FramebufferCache;

        #ifdef LD_PLATFORM_DESKTOP
        static constexpr Diligent::TEXTURE_FORMAT DepthBufferFormat = Diligent::TEX_FORMAT_D32_FLOAT;
        #endif

        #ifdef LD_PLATFORM_MOBILE
        static constexpr Diligent::TEXTURE_FORMAT DepthBufferFormat = Diligent::TEX_FORMAT_D16_UNORM;
        #endif
};
}