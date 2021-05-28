#include "Luddite/Graphics/DefferedRenderer.hpp"
#include "Luddite/Graphics/Renderer.hpp"

using namespace Diligent;
namespace Luddite
{
void DefferedRenderer::Initialize(Diligent::RefCntAutoPtr<Diligent::IRenderDevice> pDevice,
                                  Diligent::RefCntAutoPtr<Diligent::IDeviceContext> pImmediateContext,
                                  Diligent::TEXTURE_FORMAT ColorBufferFormat,
                                  Diligent::RefCntAutoPtr<Diligent::IShaderSourceInputStreamFactory> pShaderSourceFactory
                                  )
{
        m_pDevice = pDevice;
        m_pImmediateContext = pImmediateContext;
        // m_pSwapChain = pSwapChain;
        m_pShaderSourceFactory = pShaderSourceFactory;
        CreateRenderPass(ColorBufferFormat);

        {
                ShaderAttributeListDescription ConstantShaderAttributes;
                ConstantShaderAttributes.AddMat4("g_CameraViewProj");

                ShaderAttributeListDescription MaterialShaderAttributes;
                MaterialShaderAttributes.AddVec3("Diffuse");
                MaterialShaderAttributes.AddFloat("Metallic");
                MaterialShaderAttributes.AddFloat("Roughness");

                BasicShaderPipeline.Initialize(
                        m_pRenderPass,
                        "Deffered/BasicMesh.vsh",
                        "Deffered/BasicMesh.psh",
                        "BasicPBR Pipeline",
                        ConstantShaderAttributes,
                        MaterialShaderAttributes
                        );
        }
        {
                ShaderAttributeListDescription ConstantShaderAttributes;
                // ConstantShaderAttributes.AddFloat("g_AmbientPower");

                EnvironmentalLightingPipeline.Initialize(
                        m_pRenderPass,
                        "Deffered/WholeScreen.vsh",
                        // "Deffered/Debug/ViewNormal.psh",
                        "Deffered/AmbientPBR.psh",
                        "AmbientPBR Pipeline",
                        ConstantShaderAttributes,
                        static_cast<uint8_t>(G_BUFFER_FLAGS::COLOR) |
                        static_cast<uint8_t>(G_BUFFER_FLAGS::NORMAL) |
                        static_cast<uint8_t>(G_BUFFER_FLAGS::DEPTH)
                        );
                // EnvironmentalLightingPipeline.GetConstantData().SetFloat("g_AmbientPower", 0.1f);
        }
}

void DefferedRenderer::CreateRenderPass(Diligent::TEXTURE_FORMAT RTVFormat)
{
        //RenderPass Description
        constexpr Uint32 NumAttachments = 5;
        RenderPassAttachmentDesc Attachments[NumAttachments];

        //Diffuse (RGB) and Metallic (A) buffer
        Attachments[0].Format = TEX_FORMAT_RGBA8_UNORM;
        Attachments[0].InitialState = RESOURCE_STATE_RENDER_TARGET;
        Attachments[0].FinalState = RESOURCE_STATE_INPUT_ATTACHMENT;
        Attachments[0].LoadOp = ATTACHMENT_LOAD_OP_CLEAR;
        Attachments[0].StoreOp = ATTACHMENT_STORE_OP_DISCARD;

        //Normal (RGB) and Roughness (A) buffer
        Attachments[1].Format = TEX_FORMAT_RGBA8_UNORM;
        Attachments[1].InitialState = RESOURCE_STATE_RENDER_TARGET;
        Attachments[1].FinalState = RESOURCE_STATE_INPUT_ATTACHMENT;
        Attachments[1].LoadOp = ATTACHMENT_LOAD_OP_CLEAR;
        Attachments[1].StoreOp = ATTACHMENT_STORE_OP_DISCARD;

        //Z Buffer
        Attachments[2].Format = TEX_FORMAT_R32_FLOAT;
        Attachments[2].InitialState = RESOURCE_STATE_RENDER_TARGET;
        Attachments[2].FinalState = RESOURCE_STATE_INPUT_ATTACHMENT;
        Attachments[2].LoadOp = ATTACHMENT_LOAD_OP_CLEAR;
        Attachments[2].StoreOp = ATTACHMENT_STORE_OP_DISCARD;

        //Depth Buffer
        Attachments[3].Format = DepthBufferFormat;
        Attachments[3].InitialState = RESOURCE_STATE_DEPTH_WRITE;
        Attachments[3].FinalState = RESOURCE_STATE_DEPTH_WRITE;
        Attachments[3].LoadOp = ATTACHMENT_LOAD_OP_CLEAR;
        Attachments[3].StoreOp = ATTACHMENT_STORE_OP_DISCARD;

        //Frame Buffer
        Attachments[4].Format = RTVFormat;
        Attachments[4].InitialState = RESOURCE_STATE_RENDER_TARGET;
        Attachments[4].FinalState = RESOURCE_STATE_RENDER_TARGET;
        Attachments[4].LoadOp = ATTACHMENT_LOAD_OP_CLEAR;
        Attachments[4].StoreOp = ATTACHMENT_STORE_OP_STORE;

        //Sub-Passes
        constexpr Uint32 NumSubpasses = 2;
        SubpassDesc Subpasses[NumSubpasses];

        //Pass 1
        AttachmentReference RTAttachmentRefs0[] =
        {
                {0, RESOURCE_STATE_RENDER_TARGET},
                {1, RESOURCE_STATE_RENDER_TARGET},
                {2, RESOURCE_STATE_RENDER_TARGET}
        };

        AttachmentReference DepthAttachmentRef0 = {3, RESOURCE_STATE_DEPTH_WRITE};

        Subpasses[0].RenderTargetAttachmentCount = _countof(RTAttachmentRefs0);
        Subpasses[0].pRenderTargetAttachments = RTAttachmentRefs0;
        Subpasses[0].pDepthStencilAttachment = &DepthAttachmentRef0;

        //Pass 2
        AttachmentReference RTAttachmentRefs1[] =
        {
                {4, RESOURCE_STATE_RENDER_TARGET}
        };

        AttachmentReference DepthAttachmentRef1 = {3, RESOURCE_STATE_DEPTH_WRITE};

        AttachmentReference InputAttachmentRefs1[] =
        {
                {0, RESOURCE_STATE_INPUT_ATTACHMENT},
                {1, RESOURCE_STATE_INPUT_ATTACHMENT},
                {2, RESOURCE_STATE_INPUT_ATTACHMENT}
        };

        Subpasses[1].RenderTargetAttachmentCount = _countof(RTAttachmentRefs1);
        Subpasses[1].pRenderTargetAttachments = RTAttachmentRefs1;
        Subpasses[1].pDepthStencilAttachment = &DepthAttachmentRef1;
        Subpasses[1].InputAttachmentCount = _countof(InputAttachmentRefs1);
        Subpasses[1].pInputAttachments = InputAttachmentRefs1;

        //Dependencies
        SubpassDependencyDesc Dependencies[1];
        Dependencies[0].SrcSubpass = 0;
        Dependencies[0].DstSubpass = 1;
        Dependencies[0].SrcStageMask = PIPELINE_STAGE_FLAG_RENDER_TARGET;
        Dependencies[0].DstStageMask = PIPELINE_STAGE_FLAG_PIXEL_SHADER;
        Dependencies[0].SrcAccessMask = ACCESS_FLAG_RENDER_TARGET_WRITE;
        Dependencies[0].DstAccessMask = ACCESS_FLAG_SHADER_READ;

        RenderPassDesc RPDesc;
        RPDesc.Name = "Deferred shading render pass desc";
        RPDesc.AttachmentCount = _countof(Attachments);
        RPDesc.pAttachments = Attachments;
        RPDesc.SubpassCount = _countof(Subpasses);
        RPDesc.pSubpasses = Subpasses;
        RPDesc.DependencyCount = _countof(Dependencies);
        RPDesc.pDependencies = Dependencies;

        m_pDevice->CreateRenderPass(RPDesc, &m_pRenderPass);
        VERIFY_EXPR(m_pRenderPass != nullptr);
}

DefferedRenderer::FrameBufferData DefferedRenderer::CreateFramebuffer()
{
        const auto& RPDesc = m_pRenderPass->GetDesc();
        const auto& RTDesc = m_pCurrentRenderTarget.RTV->GetDesc();
        // const auto& SCDesc = m_pSwapChain->GetDesc();

        // Create window-size offscreen render target
        TextureDesc TexDesc;
        TexDesc.Name = "Color G-buffer";
        TexDesc.Type = RESOURCE_DIM_TEX_2D;
        TexDesc.BindFlags = BIND_RENDER_TARGET | BIND_INPUT_ATTACHMENT;
        TexDesc.Format = RPDesc.pAttachments[0].Format;
        TexDesc.Width = m_pCurrentRenderTarget.width;
        TexDesc.Height = m_pCurrentRenderTarget.height;
        TexDesc.MipLevels = 1;

        // Define optimal clear value
        TexDesc.ClearValue.Format = TexDesc.Format;
        TexDesc.ClearValue.Color[0] = 0.f;
        TexDesc.ClearValue.Color[1] = 0.f;
        TexDesc.ClearValue.Color[2] = 0.f;
        TexDesc.ClearValue.Color[3] = 1.f;
        RefCntAutoPtr<ITexture> pColorBuffer;
        m_pDevice->CreateTexture(TexDesc, nullptr, &pColorBuffer);

        Diligent::ITextureView* pDstRenderTarget;
        // OpenGL does not allow combining swap chain render target with any
        // other render target, so we have to create an auxiliary texture.
        RefCntAutoPtr<ITexture> pOpenGLOffsreenRenderTarget;
        if (m_pDevice->GetDeviceCaps().IsGLDevice() && m_pCurrentRenderTarget.is_swap_chain_buffer)
        {
                TexDesc.Name = "OpenGL Color Offscreen Render Target";
                TexDesc.Format = Renderer::GetDefaultRTVFormat();
                m_pDevice->CreateTexture(TexDesc, nullptr, &pOpenGLOffsreenRenderTarget);
                pDstRenderTarget = pOpenGLOffsreenRenderTarget->GetDefaultView(TEXTURE_VIEW_RENDER_TARGET);
        }
        else
        {
                pDstRenderTarget = m_pCurrentRenderTarget.RTV;
        }

        TexDesc.Name = "Normal G-buffer";
        TexDesc.Type = RESOURCE_DIM_TEX_2D;
        TexDesc.BindFlags = BIND_RENDER_TARGET | BIND_INPUT_ATTACHMENT;
        TexDesc.Format = RPDesc.pAttachments[1].Format;
        TexDesc.Width = m_pCurrentRenderTarget.width;
        TexDesc.Height = m_pCurrentRenderTarget.height;
        TexDesc.MipLevels = 1;

        // Define optimal clear value
        TexDesc.ClearValue.Format = TexDesc.Format;
        TexDesc.ClearValue.Color[0] = 0.f;
        TexDesc.ClearValue.Color[1] = 0.f;
        TexDesc.ClearValue.Color[2] = 0.f;
        TexDesc.ClearValue.Color[3] = 1.f;
        RefCntAutoPtr<ITexture> pNormalBuffer;
        m_pDevice->CreateTexture(TexDesc, nullptr, &pNormalBuffer);

        TexDesc.Name = "Depth Z G-buffer";
        TexDesc.Format = RPDesc.pAttachments[2].Format;

        TexDesc.ClearValue.Format = TexDesc.Format;
        TexDesc.ClearValue.Color[0] = 1.f;
        TexDesc.ClearValue.Color[1] = 1.f;
        TexDesc.ClearValue.Color[2] = 1.f;
        TexDesc.ClearValue.Color[3] = 1.f;
        RefCntAutoPtr<ITexture> pDepthZBuffer;
        m_pDevice->CreateTexture(TexDesc, nullptr, &pDepthZBuffer);


        TexDesc.Name = "Depth buffer";
        TexDesc.Format = RPDesc.pAttachments[3].Format;
        TexDesc.BindFlags = BIND_DEPTH_STENCIL;

        TexDesc.ClearValue.Format = TexDesc.Format;
        TexDesc.ClearValue.DepthStencil.Depth = 1.f;
        TexDesc.ClearValue.DepthStencil.Stencil = 0;
        RefCntAutoPtr<ITexture> pDepthBuffer;
        m_pDevice->CreateTexture(TexDesc, nullptr, &pDepthBuffer);


        ITextureView* pAttachments[] = //
        {
                pColorBuffer->GetDefaultView(TEXTURE_VIEW_RENDER_TARGET),
                pNormalBuffer->GetDefaultView(TEXTURE_VIEW_RENDER_TARGET),
                pDepthZBuffer->GetDefaultView(TEXTURE_VIEW_RENDER_TARGET),
                pDepthBuffer->GetDefaultView(TEXTURE_VIEW_DEPTH_STENCIL),
                pDstRenderTarget //
        };

        FramebufferDesc FBDesc;
        FBDesc.Name = "G-buffer framebuffer";
        FBDesc.pRenderPass = m_pRenderPass;
        FBDesc.AttachmentCount = _countof(pAttachments);
        FBDesc.ppAttachments = pAttachments;

        RefCntAutoPtr<IFramebuffer> pFrameBuffer;
        m_pDevice->CreateFramebuffer(FBDesc, &pFrameBuffer);
        VERIFY_EXPR(pFrameBuffer != nullptr);


        int SRBIndex = EnvironmentalLightingPipeline.CreateSRB(
                pColorBuffer->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE),
                pNormalBuffer->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE),
                pDepthZBuffer->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE)
                );

        return {pFrameBuffer, pOpenGLOffsreenRenderTarget, SRBIndex};
}

DefferedRenderer::FrameBufferData* DefferedRenderer::GetCurrentFramebufferData()
{
        // auto* pCurrentBackBufferRTV = ( m_pCurrentRenderTarget->is_swap_chain_buffer) ?
        //                               nullptr :
        //                               m_pCurrentRenderTarget->RTV;


        auto fb_it = m_FramebufferCache.find(m_pCurrentRenderTarget.RTV);
        if (fb_it != m_FramebufferCache.end())
        {
                return &fb_it->second;
        }
        else
        {
                auto it = m_FramebufferCache.emplace(m_pCurrentRenderTarget.RTV, CreateFramebuffer());
                VERIFY_EXPR(it.second);
                return &it.first->second;
        }
}

void DefferedRenderer::PrepareDraw(RenderTarget& render_target)
{
        m_pCurrentRenderTarget = render_target;
        m_DrawPeriodFrameBufferData = GetCurrentFramebufferData();

        BeginRenderPassAttribs RPBeginInfo;
        RPBeginInfo.pRenderPass = m_pRenderPass;
        RPBeginInfo.pFramebuffer = m_DrawPeriodFrameBufferData->pFrameBuffer;

        OptimizedClearValue ClearValues[5];
        // Color
        ClearValues[0].Color[0] = 0.f;
        ClearValues[0].Color[1] = 0.f;
        ClearValues[0].Color[2] = 0.f;
        ClearValues[0].Color[3] = 0.f;

        // NormalBasicShaderPipeline
        ClearValues[1].Color[0] = 0.f;
        ClearValues[1].Color[1] = 0.f;
        ClearValues[1].Color[2] = 0.f;
        ClearValues[1].Color[3] = 0.f;

        // Depth Z
        ClearValues[2].Color[0] = 1.f;
        ClearValues[2].Color[1] = 1.f;
        ClearValues[2].Color[2] = 1.f;
        ClearValues[2].Color[3] = 1.f;

        // Depth buffer
        ClearValues[3].DepthStencil.Depth = 1.f;

        // Final color buffer
        ClearValues[4].Color[0] = 0.0625f;
        ClearValues[4].Color[1] = 0.1625f;
        ClearValues[4].Color[2] = 0.1625f;
        ClearValues[4].Color[3] = 1.f;

        RPBeginInfo.pClearValues = ClearValues;
        RPBeginInfo.ClearValueCount = _countof(ClearValues);
        RPBeginInfo.StateTransitionMode = RESOURCE_STATE_TRANSITION_MODE_TRANSITION;
        m_pImmediateContext->BeginRenderPass(RPBeginInfo);
}

void DefferedRenderer::ApplyLighting()
{
        m_pImmediateContext->NextSubpass();

        EnvironmentalLightingPipeline.PrepareDraw(m_DrawPeriodFrameBufferData->SRBIndex);
        EnvironmentalLightingPipeline.Draw();

        // // Set the lighting PSO
        // m_pImmediateContext->SetPipelineState(m_pAmbientLightPSO);
        // // Commit shader resources
        // m_pImmediateContext->CommitShaderResources(m_pAmbientLightSRB, RESOURCE_STATE_TRANSITION_MODE_VERIFY);
        // {
        //         // Draw quad
        //         DrawAttribs DrawAttrs;
        //         DrawAttrs.NumVertices = 4;
        //         DrawAttrs.Flags = DRAW_FLAG_VERIFY_ALL; // Verify the state of vertex and index buffers
        //         m_pImmediateContext->Draw(DrawAttrs);
        // }
}

void DefferedRenderer::FinalizeDraw()
{
        m_pImmediateContext->EndRenderPass();

        if (m_pDevice->GetDeviceCaps().IsGLDevice() && m_pCurrentRenderTarget.is_swap_chain_buffer)
        {
                // LD_LOG_TRACE("COPYING FROM OFFSCREEN TEXTURE");
                // In OpenGL we now have to copy our off-screen buffer to the default framebuffer
                auto pOffscreenRenderTarget = m_DrawPeriodFrameBufferData->pOpenGLRenderTexture;//pFrameBuffer->GetDesc().ppAttachments[4]->GetTexture();
                auto* pBackBuffer = m_pCurrentRenderTarget.RTV->GetTexture();

                CopyTextureAttribs CopyAttribs{pOffscreenRenderTarget, RESOURCE_STATE_TRANSITION_MODE_TRANSITION,
                                               pBackBuffer, RESOURCE_STATE_TRANSITION_MODE_TRANSITION};
                m_pImmediateContext->CopyTexture(CopyAttribs);
        }
}

void DefferedRenderer::OnWindowResize(int width, int height)
{
        ReleaseWindowResources();
}
void DefferedRenderer::ReleaseWindowResources()
{
        // LD_LOG_INFO("released window resources");
        // m_DrawPeriodFrameBufferData->pFrameBuffer.Release();
        // m_DrawPeriodFrameBufferData->pOpenGLRenderTexture.Release();
        for (auto& fb : m_FramebufferCache)
        {
                fb.second.pFrameBuffer.Release();
                fb.second.pOpenGLRenderTexture.Release();
        }
        m_FramebufferCache.clear();
        EnvironmentalLightingPipeline.ClearSRBs();
}
}