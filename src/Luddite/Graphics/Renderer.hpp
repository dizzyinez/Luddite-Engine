#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/Core/Core.hpp"

#include "Luddite/Graphics/DiligentInclude.hpp"
#include "Luddite/Graphics/Texture.hpp"
#include "Luddite/Graphics/QuadBatchRenderer.hpp"
#include "Luddite/Graphics/PBRRenderer.hpp"
#include "Luddite/Graphics/DefferedRenderer.hpp"
#include "Luddite/Graphics/ModelLibrary.hpp"
#include "Luddite/Graphics/RenderTarget.hpp"
// #include "Luddite/Graphics/"

// #include "Common/interface/BasicMath.hpp"

constexpr float DefaultClearColor[4] = {0.f, 0.f, 0.f, 0.f};

namespace Luddite
{
// class LUDDITE_API Application;
class LUDDITE_API Renderer
{
        public:
        static void Initialize();
        static void BeginScene();
        static void SubmitMesh(BasicMeshHandle mesh, const glm::mat4& transform);
        static void EndScene();
        static void Draw(RenderTarget& render_target, const Camera& camera);
        static void Present();

        static inline Diligent::RefCntAutoPtr<Diligent::IRenderDevice>&  GetDevice() {return m_pDevice;}
        static inline Diligent::RefCntAutoPtr<Diligent::IDeviceContext>& GetContext() {return m_pImmediateContext;}
        static inline Diligent::RefCntAutoPtr<Diligent::IEngineFactory>& GetEngineFactory() {return m_pEngineFactory;}
        static inline Diligent::RefCntAutoPtr<Diligent::IShaderSourceInputStreamFactory>& GetShaderSourceFactory() {return m_pShaderSourceFactory;}
        static inline void SetDefaultRTVFormat(Diligent::TEXTURE_FORMAT format) {m_DefaultRTVFormat = format;}
        static inline Diligent::TEXTURE_FORMAT GetDefaultRTVFormat() {return m_DefaultRTVFormat;}

        static void BindRenderTarget(RenderTarget& render_target);
        static void ClearRenderTarget(RenderTarget& render_target, const float clear_color[4] = DefaultClearColor);
        static RenderTexture CreateRenderTexture(uint32_t width, uint32_t height,
                                                 Diligent::TEXTURE_FORMAT color_format = m_DefaultRTVFormat,
                                                 Diligent::TEXTURE_FORMAT depth_format = Diligent::TEX_FORMAT_D32_FLOAT);
        static void TransitionRenderTextureToRenderTarget(RenderTexture& RenderTexture);
        static void TransitionRenderTextureToShaderResource(RenderTexture& RenderTexture);
        static void ReleaseBufferResources();


        private:
        friend class Application;
        friend class Window;
        static void SetMatricies();
        static void PrepareDraw();
        static void OnWindowResize(int width, int height);
        static inline Diligent::RefCntAutoPtr<Diligent::IRenderDevice>  m_pDevice;
        static inline Diligent::RefCntAutoPtr<Diligent::IDeviceContext> m_pImmediateContext;
        static inline Diligent::RefCntAutoPtr<Diligent::IEngineFactory> m_pEngineFactory;
        static inline Diligent::RefCntAutoPtr<Diligent::IShaderSourceInputStreamFactory> m_pShaderSourceFactory;
        static inline Diligent::TEXTURE_FORMAT m_DefaultRTVFormat;
        // Diligent::float4x4 m_WorldViewProjMatrix;
        // float accum = 0.0f;

        // static inline DefferedPipelineState m_PipelineState;
        static inline DefferedRenderer m_DefferedRenderer;
        static inline QuadBatchRenderer m_basic_quad_renderer;
        static inline PBRRenderer m_PBRRenderer;
        // static inline ModelLoader m_ModelLoader;

        static inline struct RenderScene
        {
                std::unordered_map<BasicMeshHandle, std::vector<glm::mat4> > m_BasicMeshes;
        } m_RenderScene;

        // Window* m_pWindow;
        // Texture t;
};
}