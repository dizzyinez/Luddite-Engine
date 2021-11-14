#include "Luddite/Graphics/Renderer.hpp"
#include "Graphics/GraphicsTools/interface/MapHelper.hpp"
#include "Luddite/Platform/Window/Window.hpp"
#include <mutex>


namespace Luddite
{
// MaterialHandle awesome_material;
void Renderer::Initialize()
{
        std::lock_guard<std::mutex> lock{m_Mutex};
        m_pEngineFactory->CreateDefaultShaderSourceStreamFactory("./Assets/Shaders/;./Assets/Shaders/", &m_pShaderSourceFactory);

        // m_basic_quad_renderer.Initialize(m_pDevice,
        //         m_pImmediateContext,
        //         m_pSwapChain->GetDesc().ColorBufferFormat,
        //         m_pSwapChain->GetDesc().DepthBufferFormat,
        //         m_pShaderSourceFactory,
        //         "quad_inst.vsh",
        //         "quad_inst.psh"
        //         );


        m_VTFSRenderer.Initialize(m_DefaultRTVFormat);
        // m_DefferedRenderer.Initialize(m_DefaultRTVFormat);
        // awesome_material = m_DefferedRenderer.BasicShaderPipeline.GetMaterial("AWESOME");
        // awesome_material->m_data.SetVec3("Diffuse", glm::vec3(0.2f, 0.2f, 0.2f));
        // awesome_material->m_data.SetFloat("Metallic", 1.0f);
        // awesome_material->m_data.SetFloat("Roughness", 0.f);

        // {
        //         // Diligent::TextureLoadInfo loadInfo;
        //         // loadInfo.IsSRGB = false;
        //         // loadInfo.GenerateMips = false;
        //         // loadInfo.Name = "ENVIRONMENTAL CUBEMAP";
        //         // loadInfo.Format = Diligent::TEX_FORMAT_RGBA16_FLOAT;
        //         // Diligent::RefCntAutoPtr<Diligent::ITexture> awesome_cubemap         // Diligent::CreateTextureFromFile("./Assets/irradiance.dds", loadInfo, Renderer::GetDevice(), &awesome_cubemap);
        //         Texture::Handle ibl_texture = Assets::GetTextureLibrary().GetAsset(0);
        //         ibl_texture.get()->TransitionToShaderResource();
        //         m_DefferedRenderer.EnvironmentalLightingPipeline.GetConstantData().SetTexture("g_IrradianceMap", ibl_texture);
        // }

        // {
        //         // Diligent::TextureLoadInfo loadInfo;
        //         // loadInfo.IsSRGB = false;
        //         // loadInfo.GenerateMips = false;
        //         // loadInfo.Name = "ENVIRONMENTAL CUBEMAP";
        //         // loadInfo.Format = Diligent::TEX_FORMAT_RGBA16_FLOAT;
        //         // Diligent::RefCntAutoPtr<Diligent::ITexture> awesome_cubemap;
        //         // Diligent::CreateTextureFromFile("./Assets/radiance.dds", loadInfo, Renderer::GetDevice(), &awesome_cubemap);
        //         Texture::Handle ibl_texture = Assets::GetTextureLibrary().GetAsset(0);
        //         ibl_texture.get()->TransitionToShaderResource();
        //         m_DefferedRenderer.EnvironmentalLightingPipeline.GetConstantData().SetTexture("g_RadianceMap", ibl_texture);
        // }

        // {
        //         // Diligent::TextureLoadInfo loadInfo;
        //         // loadInfo.IsSRGB = false;
        //         // loadInfo.GenerateMips = false;
        //         // loadInfo.Name = "ENVIRONMENTAL CUBEMAP";
        //         // loadInfo.Format = Diligent::TEX_FORMAT_RGBA16_FLOAT;
        //         // Diligent::RefCntAutoPtr<Diligent::ITexture> awesome_cubemap;
        //         // Diligent::CreateTextureFromFile("./Assets/environment.dds", loadInfo, Renderer::GetDevice(), &awesome_cubemap);
        //         Texture::Handle ibl_texture = Assets::GetTextureLibrary().GetAsset(0);
        //         ibl_texture.get()->TransitionToShaderResource();
        //         m_DefferedRenderer.EnvironmentalLightingPipeline.GetConstantData().SetTexture("g_Skybox", ibl_texture);
        // }
        // m_RenderScene.m_PointLights.push_back({glm::vec4(0.f, 0.f, 0.f, 1.f), glm::vec3(1.f, 0.f, 0.f), 1.f, 1.f});
        m_RenderScene.m_PointLights.push_back({glm::vec4(1.f, 0.f, 0.f, 1.f), glm::vec3(1.f, 1.f, 0.f), 8.f, 1.f});
        m_RenderScene.m_PointLights.push_back({glm::vec4(0.f, 1.f, 0.f, 1.f), glm::vec3(1.f, 0.f, 1.f), 8.f, 1.f});
        m_RenderScene.m_PointLights.push_back({glm::vec4(0.f, 0.f, 1.f, 1.f), glm::vec3(0.f, 1.f, 1.f), 8.f, 1.f});
        m_RenderScene.m_PointLights.push_back({glm::vec4(1.f, 1.f, 1.f, 1.f), glm::vec3(1.f, 0.f, 0.f), 8.f, 1.f});
        m_RenderScene.m_PointLights.push_back({glm::vec4(10.f, 0.f, 0.f, 1.f), glm::vec3(1.f, 1.f, 1.f), 10.f, 1.f});
}

void Renderer::BeginScene()
{
        std::lock_guard<std::mutex> lock{m_Mutex};
        m_RenderScene.m_Meshes.clear();
        m_RenderScene.m_PointLights.clear();
        m_RenderScene.m_SpotLights.clear();
        m_RenderScene.m_DirectionalLights.clear();
        // for (auto pair : m_RenderScene.m_BasicMeshes)
        // {
        //         LD_LOG_INFO("CLEAR");
        //         pair.second.clear();
        // }
}

void Renderer::SubmitMesh(Mesh* mesh, const glm::mat4& transform, Handle<Material> material)
{
        std::lock_guard<std::mutex> lock{m_Mutex};
        m_RenderScene.m_Meshes[material].push_back(std::make_pair(mesh, transform));
}

void Renderer::SubmitPointLight(const PointLightCPU& point_light)
{
        std::lock_guard<std::mutex> lock{m_Mutex};
        m_RenderScene.m_PointLights.push_back(point_light);
}
void Renderer::SubmitSpotLight(const SpotLightCPU& spot_light)
{
        std::lock_guard<std::mutex> lock{m_Mutex};
        m_RenderScene.m_SpotLights.push_back(spot_light);
}
void Renderer::SubmitDirectionalLight(const DirectionalLightCPU& directional_light)
{
        std::lock_guard<std::mutex> lock{m_Mutex};
        m_RenderScene.m_DirectionalLights.push_back(directional_light);
}

void Renderer::EndScene()
{
}

void Renderer::BindRenderTarget(RenderTarget& render_target)
{
        std::lock_guard<std::mutex> lock{m_Mutex};
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
        TexDesc.Name = "Unnamed Render Texture";
        TexDesc.Type = Diligent::RESOURCE_DIM_TEX_2D;
        TexDesc.Width = width;
        TexDesc.Height = height;

        // TexDesc.CPUAccessFlags = Diligent::CPU_ACCESS_NONE;
        TexDesc.BindFlags = Diligent::BIND_SHADER_RESOURCE | Diligent::BIND_RENDER_TARGET | Diligent::BIND_UNORDERED_ACCESS;
        TexDesc.Format = color_format;

        // m_pTexture
        m_pDevice->CreateTexture(TexDesc, nullptr, &rt.m_pTexture);
        rt.m_pRTV = rt.m_pTexture->GetDefaultView(Diligent::TEXTURE_VIEW_RENDER_TARGET);
        rt.m_pSRV = rt.m_pTexture->GetDefaultView(Diligent::TEXTURE_VIEW_SHADER_RESOURCE);

        TexDesc.BindFlags = Diligent::BIND_DEPTH_STENCIL;
        TexDesc.Format = depth_format;
        m_pDevice->CreateTexture(TexDesc, nullptr, &rt.m_pDepthStencil);
        rt.m_pDSV = rt.m_pDepthStencil->GetDefaultView(Diligent::TEXTURE_VIEW_DEPTH_STENCIL);


        rt.m_RenderTarget.RTV = rt.m_pRTV;
        rt.m_RenderTarget.DSV = rt.m_pDSV;
        rt.m_RenderTarget.width = width;
        rt.m_RenderTarget.height = height;

        return rt;
}
void Renderer::Draw(const RenderTarget& render_target, const Camera& camera)
{
        std::lock_guard<std::mutex> lock{m_Mutex};
        // SetMatricies();
        // BindRenderTarget(render_target);

        // m_DefferedRenderer.PrepareDraw(render_target);

        //TEMP
        // glm::mat4 view = glm::lookAt(glm::vec3(0, 0, -5.f), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        // auto Proj = glm::perspective(glm::radians(90.f), 1.f, 0.1f, 1000.f);
        // auto vpbad = Proj * view;

        auto projection = render_target.GetProjectionMatrix(camera);
        auto view = render_target.GetViewMatrix(camera);
        auto view_projection = projection * view;
        auto inverse_projection = glm::inverse(projection);
        auto inverse_view = glm::inverse(view);

        m_VTFSRenderer.Render(render_target, camera, m_RenderScene);

        // m_VTFSRenderer.ComputeGridFrustums(render_target, inverse_view);

        // m_DefferedRenderer.BasicShaderPipeline.GetConstantDataDesc().SetMat4(m_DefferedRenderer.BasicShaderPipeline.GetConstantData(), "g_CameraViewProj", view_projection);



        // m_DefferedRenderer.BasicShaderPipeline.PrepareDraw();
        // // m_DefferedRenderer.BasicShaderPipeline.SetMaterial(awesome_material);
        // for (auto pair : m_RenderScene.m_BasicMeshes)
        // {
        //         auto& mesh = pair.first;
        //         Diligent::Uint32 offset = 0;
        //         Diligent::IBuffer* pBuffs[] = {mesh->m_pVertexBuffer};
        //         Renderer::GetContext()->SetVertexBuffers(0, 1, pBuffs, &offset, Diligent::RESOURCE_STATE_TRANSITION_MODE_VERIFY, Diligent::SET_VERTEX_BUFFERS_FLAG_RESET);
        //         Renderer::GetContext()->SetIndexBuffer(mesh->m_pIndexBuffer, 0, Diligent::RESOURCE_STATE_TRANSITION_MODE_VERIFY);
        //         for (auto& transform : pair.second)
        //         {
        //                 m_DefferedRenderer.BasicShaderPipeline.GetModelDataDesc().SetMat4(m_DefferedRenderer.BasicShaderPipeline.GetModelData(), "g_Transform", transform);
        //                 m_DefferedRenderer.BasicShaderPipeline.UploadModelData();


        //                 //For textures
        //                 // Renderer::GetContext()->CommitShaderResources(m_pCurrentMaterial->m_pMaterialConstantsBuffer->)

        //                 Diligent::DrawIndexedAttribs DrawAttrs;
        //                 DrawAttrs.NumIndices = mesh->indicies.size();
        //                 DrawAttrs.IndexType = Diligent::VT_UINT32;
        //                 DrawAttrs.Flags = Diligent::DRAW_FLAG_VERIFY_ALL;
        //                 Renderer::GetContext()->DrawIndexed(DrawAttrs);
        //         }
        // }

        // m_DefferedRenderer.EnvironmentalLightingPipeline.GetConstantDataDesc().SetMat4(m_DefferedRenderer.EnvironmentalLightingPipeline.GetConstantData(), "g_InverseProjectionMatrix", inverse_projection);
        // m_DefferedRenderer.EnvironmentalLightingPipeline.GetConstantDataDesc().SetMat4(m_DefferedRenderer.EnvironmentalLightingPipeline.GetConstantData(), "g_InverseViewMatrix", inverse_view);

        // m_DefferedRenderer.ApplyLighting();

        // m_DefferedRenderer.FinalizeDraw();

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
        std::lock_guard<std::mutex> lock{m_Mutex};
        ReleaseBufferResources();
}
void Renderer::ReleaseBufferResources()
{
        m_VTFSRenderer.ReleaseAllRenderTargetResources();
        // m_DefferedRenderer.ReleaseWindowResources();
}
}
