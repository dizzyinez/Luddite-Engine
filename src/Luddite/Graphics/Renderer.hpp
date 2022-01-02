#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/Core/Core.hpp"

#include "Luddite/Graphics/DiligentInclude.hpp"
#include "Luddite/Graphics/Texture.hpp"
#include "Luddite/Graphics/QuadBatchRenderer.hpp"
#include "Luddite/Graphics/DefferedRenderer.hpp"
#include "Luddite/Graphics/VTFSRenderer.hpp"
#include "Luddite/Graphics/RenderTarget.hpp"
#include "Luddite/Core/AssetTypes/Model.hpp"
#include "Luddite/Core/AssetTypes/Shader.hpp"
#include "Luddite/Graphics/Lights.hpp"
// #include "Luddite/Graphics/"

// #include "Common/interface/BasicMath.hpp"

constexpr float DefaultClearColor[4] = {0.f, 0.f, 0.f, 0.f};

namespace Luddite
{
struct MeshEntry
{
        Mesh* mesh;
        glm::mat4 transform;
        const void* bones_matrices_key;
};

struct RenderScene
{
        std::unordered_map<Handle<Material>, std::vector<MeshEntry> > m_Meshes;
        std::unordered_map<const void*, std::vector<glm::mat4> > m_BoneTransforms;
        std::vector<PointLightCPU> m_PointLights;
        std::vector<SpotLightCPU> m_SpotLights;
        std::vector<DirectionalLightCPU> m_DirectionalLights;
};

// class LUDDITE_API Application;
class LUDDITE_API Renderer
{
        public:
        static void Initialize();
        static void BeginScene();
        static void SubmitMesh(Mesh* mesh, const glm::mat4& transform, Handle<Material> material, const std::vector<glm::mat4>* bone_matrices);
        static void SubmitPointLight(const PointLightCPU& point_light);
        static void SubmitSpotLight(const SpotLightCPU& spot_light);
        static void SubmitDirectionalLight(const DirectionalLightCPU& directional_light);
        static void EndScene();
        static void Draw(const RenderTarget& render_target, const Camera& camera);
        static void Present();

        static inline Diligent::RefCntAutoPtr<Diligent::IRenderDevice>&  GetDevice() {return m_pDevice;}
        static inline Diligent::RefCntAutoPtr<Diligent::IDeviceContext>& GetContext() {return m_pImmediateContext;}
        static inline Diligent::RefCntAutoPtr<Diligent::IEngineFactory>& GetEngineFactory() {return m_pEngineFactory;}
        static inline Diligent::RefCntAutoPtr<Diligent::IShaderSourceInputStreamFactory>& GetShaderSourceFactory() {return m_pShaderSourceFactory;}
        static inline void SetDefaultRTVFormat(Diligent::TEXTURE_FORMAT format) {m_DefaultRTVFormat = format;}
        static inline void SetDefaultDSVFormat(Diligent::TEXTURE_FORMAT format) {m_DefaultDSVFormat = format;}
        static inline Diligent::TEXTURE_FORMAT GetDefaultRTVFormat() {return m_DefaultRTVFormat;}
        static inline Diligent::TEXTURE_FORMAT GetDefaultDSVFormat() {return m_DefaultDSVFormat;}

        static void BindRenderTarget(RenderTarget& render_target);
        static void ClearRenderTarget(RenderTarget& render_target, const float clear_color[4] = DefaultClearColor);
        static RenderTexture CreateRenderTexture(uint32_t width, uint32_t height,
                                                 Diligent::TEXTURE_FORMAT color_format = m_DefaultRTVFormat,
                                                 Diligent::TEXTURE_FORMAT depth_format = m_DefaultDSVFormat);
        static void TransitionRenderTextureToRenderTarget(RenderTexture& RenderTexture);
        static void TransitionRenderTextureToShaderResource(RenderTexture& RenderTexture);
        static void ReleaseBufferResources();

        // static inline struct Settings
        // {
        //         bool MSAA_Enabled = true;
        // } m_Settings;

        private:
        friend class Application;
        friend class Window;
        static void PrepareDraw();
        static void OnWindowResize(int width, int height);
        static inline Diligent::RefCntAutoPtr<Diligent::IRenderDevice>  m_pDevice;
        static inline Diligent::RefCntAutoPtr<Diligent::IDeviceContext> m_pImmediateContext;
        static inline Diligent::RefCntAutoPtr<Diligent::IEngineFactory> m_pEngineFactory;
        static inline Diligent::RefCntAutoPtr<Diligent::IShaderSourceInputStreamFactory> m_pShaderSourceFactory;
        static inline Diligent::TEXTURE_FORMAT m_DefaultRTVFormat;
        static inline Diligent::TEXTURE_FORMAT m_DefaultDSVFormat;
        // Diligent::float4x4 m_WorldViewProjMatrix;
        // float accum = 0.0f;

        static inline VTFSRenderer m_VTFSRenderer;
        static inline QuadBatchRenderer m_basic_quad_renderer;
        // static inline ModelLoader m_ModelLoader;

        static inline RenderScene m_RenderScene;

        // Window* m_pWindow;
        // Texture t;
        static inline std::mutex m_Mutex;
};
}
