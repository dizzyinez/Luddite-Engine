#include "Luddite/Graphics/Renderer.hpp"
#include "Graphics/GraphicsTools/interface/MapHelper.hpp"
#include "Luddite/Platform/Window/Window.hpp"

namespace Luddite
{
// using namespace Diligent;
glm::mat4x4 Renderer::GetAdjustedProjectionMatrix(float FOV, float NearPlane, float FarPlane) const
{
        const auto& SCDesc = m_pSwapChain->GetDesc();

        float AspectRatio = static_cast<float>(SCDesc.Width) / static_cast<float>(SCDesc.Height);
        float XScale, YScale;
        if (SCDesc.PreTransform == Diligent::SURFACE_TRANSFORM_ROTATE_90 ||
            SCDesc.PreTransform == Diligent::SURFACE_TRANSFORM_ROTATE_270 ||
            SCDesc.PreTransform == Diligent::SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90 ||
            SCDesc.PreTransform == Diligent::SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270)
        {
                // When the screen is rotated, vertical FOV becomes horizontal FOV
                XScale = 1.f / std::tan(FOV / 2.f);
                // Aspect ratio is inversed
                YScale = XScale * AspectRatio;
        }
        else
        {
                YScale = 1.f / std::tan(FOV / 2.f);
                XScale = YScale / AspectRatio;
        }


        // glm::mat4 Proj;
        // Proj[1][1] = XScale;
        // Proj[2][2] = YScale;
        // Proj.SetNearFarClipPlanes(NearPlane, FarPlane, m_pDevice->GetDeviceCaps().IsGLDevice());
        glm::mat4x4 Proj = glm::perspective(FOV, AspectRatio, NearPlane, FarPlane);
        return Proj;
}

glm::mat4x4 Renderer::GetSurfacePretransformMatrix(const glm::vec3& f3CameraViewAxis) const
{
        const auto& SCDesc = m_pSwapChain->GetDesc();
        switch (SCDesc.PreTransform)
        {
        case Diligent::SURFACE_TRANSFORM_ROTATE_90:
                // The image content is rotated 90 degrees clockwise.
                return glm::rotate(glm::half_pi<float>(), f3CameraViewAxis);

        case Diligent::SURFACE_TRANSFORM_ROTATE_180:
                // The image content is rotated 180 degrees clockwise.
                return glm::rotate(-glm::pi<float>(), f3CameraViewAxis);

        case Diligent::SURFACE_TRANSFORM_ROTATE_270:
                // The image content is rotated 270 degrees clockwise.
                return glm::rotate(-3 * glm::half_pi<float>(), f3CameraViewAxis);

        case Diligent::SURFACE_TRANSFORM_OPTIMAL:
                UNEXPECTED("SURFACE_TRANSFORM_OPTIMAL is only valid as parameter during swap chain initialization.");
                return glm::mat4(1.f);
        // return Diligent::float4x4::Identity();

        case Diligent::SURFACE_TRANSFORM_HORIZONTAL_MIRROR:
        case Diligent::SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90:
        case Diligent::SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180:
        case Diligent::SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270:
                UNEXPECTED("Mirror transforms are not supported");
                return glm::mat4x4(1.f);

        default:
                return glm::mat4x4(1.f);
        }
}


void Renderer::Initialize()
{
        m_pEngineFactory->CreateDefaultShaderSourceStreamFactory("./Assets/Shaders/;", &m_pShaderSourceFactory);

        // m_basic_quad_renderer.Initialize(m_pDevice,
        //         m_pImmediateContext,
        //         m_pSwapChain->GetDesc().ColorBufferFormat,
        //         m_pSwapChain->GetDesc().DepthBufferFormat,
        //         m_pShaderSourceFactory,
        //         "quad_inst.vsh",
        //         "quad_inst.psh"
        //         );
        SetMatricies();
        m_PBRRenderer.Initialize(
                m_pDevice,
                m_pImmediateContext,
                m_pSwapChain,
                m_pSwapChain->GetDesc().ColorBufferFormat,
                // m_pSwapChain->GetDesc().DepthBufferFormat,
                m_pShaderSourceFactory,
                "cube.vsh",
                "cube.psh"
                );

        m_ModelLoader.GetBasicModel("Assets/box_stack.obj");
        m_ModelLoader.GetBasicModel("Assets/ox_stack.obj");


        // std::stringstream FileNameSS;
        // FileNameSS << "DGLogo" << tex << ".png";
        // auto FileName = FileNameSS.str();

        // Diligent::TextureLoadInfo load_info;
        // load_info.IsSRGB = true;
        // // Diligent::RefCntAutoPtr<Diligent::ITexture> pTex;
        // Diligent::CreateTextureFromFile(Path, load_info, pDevice, &t.GetTexture);
        // return pTex;


        // RefCntAutoPtr<ITexture> SrcTex = TexturedCube::LoadTexture(m_pDevice, FileName.c_str());
        // const auto&             TexDesc = SrcTex->GetDesc();
}
void Renderer::SetMatricies()
{
        glm::mat4 view = glm::lookAt(glm::vec3(0, 0, 25.f), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        // glm::mat4 view = glm::lookAt(
        //         glm::vec3(2.5f, 2.5f, 2.0f),
        //         glm::vec3(0.0f, 0.0f, 0.0f),
        //         glm::vec3(0.0f, 0.0f, 1.0f));
        // glm::mat4 view = glm::translate(glm::vec3{0.f, 0.f, 25.f});


        // glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(m_pWindow->GetWidth()), static_cast<float>(m_pWindow->GetHeight()), 0.0f); //reminder: this function only likes floats and seems to fail with integers
        // glm::mat4 vp = projection * view;

        auto SrfPreTransform = GetSurfacePretransformMatrix(glm::vec3{0.f, 0.f, 1.f});

        // Get projection matrix adjusted to the current screen orientation
        auto Proj = GetAdjustedProjectionMatrix(glm::pi<float>() / 4.0f, 0.1f, 100.f);

        auto vp = Proj * SrfPreTransform * view;
        // m_CameraViewProjInvMatrix = m_CameraViewProjMatrix.Inverse();


        // glm::mat4 proj_test = glm::perspective(45.0f, 800.0f / 600.0f, 1.0f, 10.0f);

        // glm::mat4 vp_test = proj_test * view_test;

        m_basic_quad_renderer.SetViewProjMatrix(vp);
        m_PBRRenderer.SetViewProjMatrix(vp);
}
void Renderer::Draw()
{
        PrepareDraw();
        m_PBRRenderer.Draw();
        // m_basic_quad_renderer.StartBatch();
        // {
        //         // Texture t;
        //         m_basic_quad_renderer.AddQuad(t, glm::translate(glm::vec3(0.f)));
        // }
        // m_basic_quad_renderer.UploadBatch();
        // m_basic_quad_renderer.DrawBatch();
}
void Renderer::Present()
{
        m_pSwapChain->Present();
}
void Renderer::PrepareDraw()
{
        SetMatricies();

        //set render targets
        auto* pRTV = m_pSwapChain->GetCurrentBackBufferRTV();
        auto* pDSV = m_pSwapChain->GetDepthBufferDSV();
        m_pImmediateContext->SetRenderTargets(1, &pRTV, pDSV, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        // Clear the back buffer
        const float ClearColor[] = {0.350f, 0.350f, 0.350f, 1.0f};
        m_pImmediateContext->ClearRenderTarget(pRTV, ClearColor, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        m_pImmediateContext->ClearDepthStencil(pDSV, Diligent::CLEAR_DEPTH_FLAG, 1.f, 0, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
}
}