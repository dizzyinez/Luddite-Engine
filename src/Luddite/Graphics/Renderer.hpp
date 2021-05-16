#pragma once
#include "Luddite/pch.hpp"
#include "Luddite/Core.hpp"

#include "Luddite/Graphics/Texture.hpp"
#include "Luddite/Graphics/QuadBatchRenderer.hpp"
#include "Luddite/Graphics/PBRRenderer.hpp"
#include "Luddite/Graphics/ModelLoader.hpp"

// #include "Common/interface/BasicMath.hpp"

#include "Luddite/Platform/DiligentPlatform.hpp"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"
#include "Graphics/GraphicsEngine/interface/SwapChain.h"

#include "Common/interface/RefCntAutoPtr.hpp"

namespace Luddite
{
class LUDDITE_API Window;
class LUDDITE_API Renderer
{
public:
        Renderer(Window* pWindow) : m_pWindow(pWindow) {}
        void Initialize();
        void Draw();
        void Present();
private:
        friend class Window;
        void SetMatricies();
        void PrepareDraw();
        Diligent::RefCntAutoPtr<Diligent::IRenderDevice>  m_pDevice;
        Diligent::RefCntAutoPtr<Diligent::IDeviceContext> m_pImmediateContext;
        Diligent::RefCntAutoPtr<Diligent::ISwapChain>     m_pSwapChain;
        Diligent::RefCntAutoPtr<Diligent::IEngineFactory> m_pEngineFactory;
        Diligent::RefCntAutoPtr<Diligent::IShaderSourceInputStreamFactory> m_pShaderSourceFactory;

        Diligent::RefCntAutoPtr<Diligent::IPipelineState> m_pPSO;

        Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> m_pSRB;
        Diligent::RefCntAutoPtr<Diligent::IBuffer>                m_CubeVertexBuffer;
        Diligent::RefCntAutoPtr<Diligent::IBuffer>                m_CubeIndexBuffer;
        Diligent::RefCntAutoPtr<Diligent::IBuffer>                m_VSConstants;
        // Diligent::float4x4 m_WorldViewProjMatrix;
        // float accum = 0.0f;
        QuadBatchRenderer m_basic_quad_renderer;
        PBRRenderer m_PBRRenderer;
        ModelLoader m_ModelLoader;

        Window* m_pWindow;
        Texture t;

        glm::mat4x4 GetAdjustedProjectionMatrix(float FOV, float NearPlane, float FarPlane) const;
        glm::mat4x4 GetSurfacePretransformMatrix(const glm::vec3& f3CameraViewAxis) const;
};
}