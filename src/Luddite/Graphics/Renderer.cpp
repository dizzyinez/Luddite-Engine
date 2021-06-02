#include "Luddite/Graphics/Renderer.hpp"
#include "Graphics/GraphicsTools/interface/MapHelper.hpp"
#include "Luddite/Platform/Window/Window.hpp"


namespace Luddite
{
MaterialHandle awesome_material;
BasicModelHandle awesome_model;
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


        m_DefferedRenderer.Initialize(
                m_pDevice,
                m_pImmediateContext,
                m_DefaultRTVFormat,
                m_pShaderSourceFactory
                );
        awesome_material = m_DefferedRenderer.BasicShaderPipeline.GetMaterial("AWESOME");
        awesome_material->m_data.SetVec3("Diffuse", glm::vec3(0.5f, 0.5f, 1.f));


        awesome_model = ModelLoader::GetBasicModel("Assets/pingpong.obj");
        for (auto mesh : awesome_model->meshes)
                SubmitMesh(mesh);

        // m_pImGui->CreateDeviceObjects();

        // m_pImGui = std::make_unique<ImGuiImplDiligent>()

        // m_ModelLoader.GetBasicModel("Assets/ox_stack.obj");


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

void Renderer::SubmitMesh(BasicMeshHandle mesh)
{
        m_RenderScene.meshes.emplace_back(mesh);
}

void Renderer::SetMatricies()
{
        glm::mat4 view = glm::lookAt(glm::vec3(0, 0, 1.f), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        // glm::mat4 view = glm::lookAt(
        //         glm::vec3(2.5f, 2.5f, 2.0f),
        //         glm::vec3(0.0f, 0.0f, 0.0f),
        //         glm::vec3(0.0f, 0.0f, 1.0f));
        // glm::mat4 view = glm::translate(glm::vec3{0.f, 0.f, 25.f});


        // glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(m_pWindow->GetWidth()), static_cast<float>(m_pWindow->GetHeight()), 0.0f); //reminder: this function only likes floats and seems to fail with integers
        // glm::mat4 vp = projection * view;

        auto SrfPreTransform = glm::mat4();// GetSurfacePretransformMatrix(glm::vec3{0.f, 0.f, 1.f});

        // Get projection matrix adjusted to the current screen orientation
        auto Proj = glm::perspective(glm::radians(90.f), 1.f, 0.1f, 1000.f);
        // GetAdjustedProjectionMatrix(glm::pi<float>() / 4.0f, 0.1f, 100.f);

        auto vp = Proj * SrfPreTransform * view;
        // m_CameraViewProjInvMatrix = m_CameraViewProjMatrix.Inverse();


        // glm::mat4 proj_test = glm::perspective(45.0f, 800.0f / 600.0f, 1.0f, 10.0f);

        // glm::mat4 vp_test = proj_test * view_test;

        m_basic_quad_renderer.SetViewProjMatrix(vp);
        m_PBRRenderer.SetViewProjMatrix(vp);
}

void Renderer::BindRenderTarget(RenderTarget& render_target)
{
        m_pImmediateContext->SetRenderTargets(1, &render_target.RTV, render_target.DSV, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
}

void Renderer::ClearRenderTarget(RenderTarget& render_target, const float clear_color[4])
{
        // const float ClearColor[] = {0.350f, 0.350f, 0.350f, 1.0f};
        m_pImmediateContext->ClearRenderTarget(render_target.RTV, clear_color, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        m_pImmediateContext->ClearDepthStencil(render_target.DSV, Diligent::CLEAR_DEPTH_FLAG, 1.f, 0, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
}

void Renderer::TransitionRenderTextureToRenderTarget(RenderTexture& RenderTexture)
{
        Diligent::StateTransitionDesc Barriers[] =
        {
                {RenderTexture.m_pTexture, Diligent::RESOURCE_STATE_UNKNOWN, Diligent::RESOURCE_STATE_RENDER_TARGET, true}
        };
        m_pImmediateContext->TransitionResourceStates(_countof(Barriers), Barriers);
}

void Renderer::TransitionRenderTextureToShaderResource(RenderTexture& RenderTexture)
{
        Diligent::StateTransitionDesc Barriers[] =
        {
                {RenderTexture.m_pTexture, Diligent::RESOURCE_STATE_UNKNOWN, Diligent::RESOURCE_STATE_SHADER_RESOURCE, true}
        };
        m_pImmediateContext->TransitionResourceStates(_countof(Barriers), Barriers);
}

RenderTexture Renderer::CreateRenderTexture(uint32_t width, uint32_t height,
                                            Diligent::TEXTURE_FORMAT color_format,
                                            Diligent::TEXTURE_FORMAT depth_format)
{
        RenderTexture rt;
        Diligent::TextureDesc TexDesc;
        TexDesc.Name = "Unnamed Render Texure";
        TexDesc.Type = Diligent::RESOURCE_DIM_TEX_2D;
        TexDesc.Width = width;
        TexDesc.Height = height;

        // TexDesc.CPUAccessFlags = Diligent::CPU_ACCESS_NONE;
        TexDesc.Format = color_format;
        TexDesc.BindFlags = Diligent::BIND_SHADER_RESOURCE | Diligent::BIND_RENDER_TARGET | Diligent::BIND_UNORDERED_ACCESS;

        // m_pTexture
        m_pDevice->CreateTexture(TexDesc, nullptr, &rt.m_pTexture);
        rt.m_pRTV = rt.m_pTexture->GetDefaultView(Diligent::TEXTURE_VIEW_RENDER_TARGET);
        rt.m_pSRV = rt.m_pTexture->GetDefaultView(Diligent::TEXTURE_VIEW_SHADER_RESOURCE);

        rt.m_RenderTarget.RTV = rt.m_pRTV;
        rt.m_RenderTarget.DSV = rt.m_pDSV;
        rt.m_RenderTarget.width = width;
        rt.m_RenderTarget.height = height;

        return rt;
}
void Renderer::Draw(RenderTarget& render_target, const Camera& camera)
{
        SetMatricies();
        BindRenderTarget(render_target);

        m_DefferedRenderer.PrepareDraw(render_target);

        //TEMP
        // glm::mat4 view = glm::lookAt(glm::vec3(0, 0, -5.f), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        // auto Proj = glm::perspective(glm::radians(90.f), 1.f, 0.1f, 1000.f);
        // auto vpbad = Proj * view;

        auto vp = render_target.GetViewProjection(camera);

        m_DefferedRenderer.BasicShaderPipeline.GetConstantData().SetMat4("g_CameraViewProj", vp);



        m_DefferedRenderer.BasicShaderPipeline.PrepareDraw();
        m_DefferedRenderer.BasicShaderPipeline.SetMaterial(awesome_material);
        for (auto mesh : m_RenderScene.meshes)
                m_DefferedRenderer.BasicShaderPipeline.DrawBasicMesh(mesh);

        m_DefferedRenderer.ApplyLighting();
        m_DefferedRenderer.FinalizeDraw();

        //set render targets
        // m_DefferedRenderer.BasicShaderPipeline.SetMaterial(awesome_material);

        // Diligent::StateTransitionDesc Barriers[] = //
        // {
        //         {m_pTexture, Diligent::RESOURCE_STATE_UNKNOWN, Diligent::RESOURCE_STATE_SHADER_RESOURCE, true}
        // };

        // m_pImmediateContext->TransitionResourceStates(_countof(Barriers), Barriers);


        // const auto& SCDesc = m_pSwapChain->GetDesc();
        // // if (SCDesc.Width > 0 && SCDesc.Height > 0)
        // uint32_t TestWindowWidth = std::min(330u, SCDesc.Width);
        // // ImGui::SetNextWindowSize())

        // const ImGuiViewport* viewport = ImGui::GetMainViewport();
        // ImGui::SetNextWindowPos(viewport->WorkPos);
        // // ImGui::SetNextWindowSize(viewport->WorkSize);
        // // ImGui::SetNextWindowViewport(viewport->ID);
        // // ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        // // ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        // // window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        // // window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        // ImGui::DockSpace(ImGui::GetID("MyDockSpace"), ImVec2(0, 0), ImGuiDockNodeFlags_PassthruCentralNode);

        // ImGui::SetNextWindowSize(ImVec2(static_cast<float>(TestWindowWidth), 0), ImGuiCond_FirstUseEver);
        // ImGui::SetNextWindowPos(ImVec2(static_cast<float>(std::max(SCDesc.Width - TestWindowWidth, 10U) - 10), 10), ImGuiCond_FirstUseEver);
        // ImGui::SetNextWindowCollapsed(true, ImGuiCond_FirstUseEver);
        // bool p_open = false;

        // ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar;
        // ImGui::Begin("Test", &p_open, window_flags);
        // ImGui::SetNextItemWidth(120);
        // ImGui::Text("This is some useful text.");
        // ImGui::End();
        // ImGui::Begin("Viewport");
        // // ImGui::Image(m_pTextureSRV_Shader_Resource, ImVec2(500, 500));
        // ImGui::End();


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
        // m_pSwapChain->Present();
}
void Renderer::PrepareDraw()
{
        // const auto& SCDesc = m_pSwapChain->GetDesc();
        // m_pImGui->NewFrame(SCDesc.Width, SCDesc.Height, SCDesc.PreTransform);

        // //set render targets
        // auto* pRTV = m_pSwapChain->GetCurrentBackBufferRTV();
        // auto* pDSV = m_pSwapChain->GetDepthBufferDSV();
        // m_pImmediateContext->SetRenderTargets(1, &pRTV, pDSV, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        // // Clear the back buffer
}

void Renderer::OnWindowResize(int width, int height)
{
        m_DefferedRenderer.OnWindowResize(width, height);
}
}