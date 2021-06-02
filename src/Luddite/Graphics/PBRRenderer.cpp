#include "Luddite/Graphics/PBRRenderer.hpp"
#include "Graphics/GraphicsTools/interface/GraphicsUtilities.h"
#include "Graphics/GraphicsTools/interface/MapHelper.hpp"



// #include "Graphics/GraphicsEngine/interface/Texture.h"
#include "TextureLoader/interface/TextureUtilities.h"
#include "Luddite/Graphics/TexturedCube.hpp"
using namespace Diligent;

namespace Luddite
{
struct ShaderConstants
{
        glm::mat4x4 ViewProjMatrix;
        glm::mat4x4 ViewProjInvMatrix;
        // float4 ViewportSize;
        // int ShowLightVolumes;
};
struct ModelBuffer
{
        glm::mat4 ModelTransform;
};
void PBRRenderer::Initialize(Diligent::RefCntAutoPtr<Diligent::IRenderDevice> pDevice,
                             Diligent::RefCntAutoPtr<Diligent::IDeviceContext> pImmediateContext,
                             Diligent::RefCntAutoPtr<Diligent::ISwapChain> pSwapChain,
                             Diligent::TEXTURE_FORMAT RTVFormat,
                             //      Diligent::TEXTURE_FORMAT DSVFormat,
                             Diligent::RefCntAutoPtr<Diligent::IShaderSourceInputStreamFactory> pShaderSourceFactory,
                             const std::string& VSFilePath,
                             const std::string& PSFilePath)
{
        m_pDevice = pDevice;
        m_pImmediateContext = pImmediateContext;
        m_pSwapChain = pSwapChain;

        CreateUniformBuffer(m_pDevice, sizeof(ShaderConstants), "ShaderConstantsCB", &m_pShaderConstantsCB);
        CreateUniformBuffer(m_pDevice, sizeof(ModelBuffer), "ModelBufferCB", &m_pModelBufferCB);

        // Load textured cube
        m_CubeVertexBuffer = TexturedCube::CreateVertexBuffer(m_pDevice);
        m_CubeIndexBuffer = TexturedCube::CreateIndexBuffer(m_pDevice);
        m_CubeTextureSRV = TexturedCube::LoadTexture(m_pDevice, "Assets/awesome.jpg")->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE);
        m_CubeTextureSRV2 = TexturedCube::LoadTexture(m_pDevice, "Assets/unknown.png")->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE);

        CreateRenderPass(RTVFormat);
        CreateAmbientLightPSO(pShaderSourceFactory);
        CreatePipelineState(pShaderSourceFactory);


        // CreatePipelineState(RTVFormat, DSVFormat, pShaderSourceFactory, VSFilePath, PSFilePath);

        // m_QuadVertexBuffer = CreateVertexBuffer();
        // m_QuadIndexBuffer = CreateIndexBuffer();

        //Transition all resources to required states as no transitions are allowed within the render pass.
        StateTransitionDesc Barriers[] = //
        {
                {m_pShaderConstantsCB, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_CONSTANT_BUFFER, true},
                {m_pModelBufferCB, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_CONSTANT_BUFFER, true},
                {m_CubeVertexBuffer, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_VERTEX_BUFFER, true},
                {m_CubeIndexBuffer, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_INDEX_BUFFER, true},
                // {m_pLightsBuffer, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_VERTEX_BUFFER, true},
                {m_CubeTextureSRV->GetTexture(), RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_SHADER_RESOURCE, true}, //
                {m_CubeTextureSRV2->GetTexture(), RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_SHADER_RESOURCE, true} //
        };

        m_pImmediateContext->TransitionResourceStates(_countof(Barriers), Barriers);

        LD_LOG_INFO("PBR Renderer Initialized");
        // ReleaseWindowResources();
}
void PBRRenderer::CreateRenderPass(Diligent::TEXTURE_FORMAT RTVFormat)
{
        //RenderPass Description
        constexpr Uint32 NumAttachments = 5;
        RenderPassAttachmentDesc Attachments[NumAttachments];

        //Diffuse RGB and Metallic buffer
        Attachments[0].Format = TEX_FORMAT_RGBA8_UNORM;
        Attachments[0].InitialState = RESOURCE_STATE_RENDER_TARGET;
        Attachments[0].FinalState = RESOURCE_STATE_INPUT_ATTACHMENT;
        Attachments[0].LoadOp = ATTACHMENT_LOAD_OP_CLEAR;
        Attachments[0].StoreOp = ATTACHMENT_STORE_OP_DISCARD;

        //Normal and Roughness buffer
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
                {0, RESOURCE_STATE_SHADER_RESOURCE},
                {1, RESOURCE_STATE_SHADER_RESOURCE},
                {2, RESOURCE_STATE_SHADER_RESOURCE}
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
void PBRRenderer::CreatePipelineState(IShaderSourceInputStreamFactory* pShaderSourceFactory)
{
        GraphicsPipelineStateCreateInfo PSOCreateInfo;
        PipelineStateDesc&              PSODesc = PSOCreateInfo.PSODesc;

        // Pipeline state name is used by the engine to report issues.
        PSODesc.Name = "PBR PSO";
        PSODesc.PipelineType = Diligent::PIPELINE_TYPE_GRAPHICS;

        PSOCreateInfo.GraphicsPipeline.pRenderPass = m_pRenderPass;
        PSOCreateInfo.GraphicsPipeline.SubpassIndex = 0; // This PSO will be used within the first subpass
        // When pRenderPass is not null, all RTVFormats and DSVFormat must be TEX_FORMAT_UNKNOWN,
        // while NumRenderTargets must be 0

        PSOCreateInfo.GraphicsPipeline.PrimitiveTopology = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        // PSOCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode = CULL_MODE_BACK;
        PSOCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode = CULL_MODE_NONE;
        PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = True;

        ShaderCreateInfo ShaderCI;
        // Tell the system that the shader source code is in HLSL.
        // For OpenGL, the engine will convert this into GLSL under the hood.
        ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;

        // OpenGL backend requires emulated combined HLSL texture samplers (g_Texture + g_Texture_sampler combination)
        ShaderCI.UseCombinedTextureSamplers = true;

        ShaderCI.pShaderSourceStreamFactory = pShaderSourceFactory;
        // Create cube vertex shader
        RefCntAutoPtr<IShader> pVS;
        {
                ShaderCI.Desc.ShaderType = SHADER_TYPE_VERTEX;
                ShaderCI.EntryPoint = "main";
                ShaderCI.Desc.Name = "Cube VS";
                ShaderCI.FilePath = "cube.vsh";
                m_pDevice->CreateShader(ShaderCI, &pVS);
                VERIFY_EXPR(pVS != nullptr);
        }

        // Create cube pixel shader
        RefCntAutoPtr<IShader> pPS;
        {
                ShaderCI.Desc.ShaderType = SHADER_TYPE_PIXEL;
                ShaderCI.EntryPoint = "main";
                ShaderCI.Desc.Name = "Cube PS";
                ShaderCI.FilePath = "cube.psh";
                m_pDevice->CreateShader(ShaderCI, &pPS);
                VERIFY_EXPR(pPS != nullptr);
        }

        const LayoutElement LayoutElems[] =
        {
                LayoutElement{0, 0, 3, VT_FLOAT32, False}, // Attribute 0 - vertex position
                LayoutElement{1, 0, 2, VT_FLOAT32, False} // Attribute 1 - texture coordinates
        };

        PSOCreateInfo.pVS = pVS;
        PSOCreateInfo.pPS = pPS;

        PSOCreateInfo.GraphicsPipeline.InputLayout.LayoutElements = LayoutElems;
        PSOCreateInfo.GraphicsPipeline.InputLayout.NumElements = _countof(LayoutElems);

        // Define variable type that will be used by default
        PSODesc.ResourceLayout.DefaultVariableType = SHADER_RESOURCE_VARIABLE_TYPE_STATIC;

        // clang-format off
        ShaderResourceVariableDesc Vars[] =
        {
                {SHADER_TYPE_VERTEX, "g_ModelTransform", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                {SHADER_TYPE_PIXEL, "g_Texture", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE}
        };
        // clang-format on
        PSODesc.ResourceLayout.Variables = Vars;
        PSODesc.ResourceLayout.NumVariables = _countof(Vars);

        // clang-format off
        // Define immutable sampler for g_Texture.
        SamplerDesc SamLinearClampDesc
        {
                FILTER_TYPE_LINEAR, FILTER_TYPE_LINEAR, FILTER_TYPE_LINEAR,
                TEXTURE_ADDRESS_CLAMP, TEXTURE_ADDRESS_CLAMP, TEXTURE_ADDRESS_CLAMP
        };
        ImmutableSamplerDesc ImtblSamplers[] =
        {
                {SHADER_TYPE_PIXEL, "g_Texture", SamLinearClampDesc}
        };

        // clang-format on
        PSODesc.ResourceLayout.ImmutableSamplers = ImtblSamplers;
        PSODesc.ResourceLayout.NumImmutableSamplers = _countof(ImtblSamplers);

        m_pDevice->CreateGraphicsPipelineState(PSOCreateInfo, &m_pCubePSO);
        VERIFY_EXPR(m_pCubePSO != nullptr);

        m_pCubePSO->GetStaticVariableByName(SHADER_TYPE_VERTEX, "ShaderConstants")->Set(m_pShaderConstantsCB);
        m_pCubePSO->GetStaticVariableByName(SHADER_TYPE_VERTEX, "ModelBuffer")->Set(m_pModelBufferCB);

        m_pCubePSO->CreateShaderResourceBinding(&m_pCubeSRB, true);
        VERIFY_EXPR(m_pCubeSRB != nullptr);
        m_pCubeSRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_Texture")->Set(m_CubeTextureSRV);

        m_pCubePSO->CreateShaderResourceBinding(&m_pCubeSRB2, true);
        VERIFY_EXPR(m_pCubeSRB2 != nullptr);
        m_pCubeSRB2->GetVariableByName(SHADER_TYPE_PIXEL, "g_Texture")->Set(m_CubeTextureSRV2);
}
RefCntAutoPtr<IFramebuffer> PBRRenderer::CreateFramebuffer(ITextureView* pDstRenderTarget)
{
        const auto& RPDesc = m_pRenderPass->GetDesc();
        const auto& SCDesc = m_pSwapChain->GetDesc();
        // Create window-size offscreen render target
        TextureDesc TexDesc;
        TexDesc.Name = "Color G-buffer";
        TexDesc.Type = RESOURCE_DIM_TEX_2D;
        TexDesc.BindFlags = BIND_RENDER_TARGET | BIND_INPUT_ATTACHMENT;
        TexDesc.Format = RPDesc.pAttachments[0].Format;
        TexDesc.Width = SCDesc.Width;
        TexDesc.Height = SCDesc.Height;
        TexDesc.MipLevels = 1;

        // Define optimal clear value
        TexDesc.ClearValue.Format = TexDesc.Format;
        TexDesc.ClearValue.Color[0] = 0.f;
        TexDesc.ClearValue.Color[1] = 0.f;
        TexDesc.ClearValue.Color[2] = 0.f;
        TexDesc.ClearValue.Color[3] = 1.f;
        RefCntAutoPtr<ITexture> pColorBuffer;
        m_pDevice->CreateTexture(TexDesc, nullptr, &pColorBuffer);

        // OpenGL does not allow combining swap chain render target with any
        // other render target, so we have to create an auxiliary texture.
        RefCntAutoPtr<ITexture> pOpenGLOffsreenColorBuffer;
        if (pDstRenderTarget == nullptr)
        {
                TexDesc.Name = "OpenGL Offscreen Render Target";
                TexDesc.Format = SCDesc.ColorBufferFormat;
                m_pDevice->CreateTexture(TexDesc, nullptr, &pOpenGLOffsreenColorBuffer);
                pDstRenderTarget = pOpenGLOffsreenColorBuffer->GetDefaultView(TEXTURE_VIEW_RENDER_TARGET);
        }



        TexDesc.Name = "Normal G-buffer";
        TexDesc.Type = RESOURCE_DIM_TEX_2D;
        TexDesc.BindFlags = BIND_RENDER_TARGET | BIND_INPUT_ATTACHMENT;
        TexDesc.Format = RPDesc.pAttachments[1].Format;
        TexDesc.Width = SCDesc.Width;
        TexDesc.Height = SCDesc.Height;
        TexDesc.MipLevels = 1;

        // Define optimal clear value
        TexDesc.ClearValue.Format = TexDesc.Format;
        TexDesc.ClearValue.Color[0] = 0.f;
        TexDesc.ClearValue.Color[1] = 0.f;
        TexDesc.ClearValue.Color[2] = 0.f;
        TexDesc.ClearValue.Color[3] = 1.f;
        RefCntAutoPtr<ITexture> pNormalBuffer;
        m_pDevice->CreateTexture(TexDesc, nullptr, &pNormalBuffer);

        // OpenGL does not allow combining swap chain render target with any
        // other render target, so we have to create an auxiliary texture.
        // RefCntAutoPtr<ITexture> pOpenGLOffsreenNormalBuffer;
        // if (pDstRenderTarget == nullptr)
        // {
        //         TexDesc.Name = "OpenGL Offscreen Render Target";
        //         TexDesc.Format = SCDesc.ColorBufferFormat;
        //         m_pDevice->CreateTexture(TexDesc, nullptr, &pOpenGLOffsreenNormalBuffer);
        //         pDstRenderTarget = pOpenGLOffsreenNormalBuffer->GetDefaultView(TEXTURE_VIEW_RENDER_TARGET);
        // }



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


        // Create SRBs that reference the framebuffer textures

        // if (!m_pLightVolumeSRB)
        // {
        //         m_pLightVolumePSO->CreateShaderResourceBinding(&m_pLightVolumeSRB, true);
        //         m_pLightVolumeSRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_SubpassInputColor")->Set(pColorBuffer->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE));
        //         m_pLightVolumeSRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_SubpassInputDepthZ")->Set(pDepthZBuffer->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE));
        // }

        if (!m_pAmbientLightSRB)
        {
                m_pAmbientLightPSO->CreateShaderResourceBinding(&m_pAmbientLightSRB, true);
                m_pAmbientLightSRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_SubpassInputColor")->Set(pColorBuffer->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE));
                m_pAmbientLightSRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_SubpassInputNormal")->Set(pNormalBuffer->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE));
                m_pAmbientLightSRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_SubpassInputDepthZ")->Set(pDepthZBuffer->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE));
        }

        return pFrameBuffer;
}
IFramebuffer* PBRRenderer::GetCurrentFramebuffer()
{
        auto* pCurrentBackBufferRTV = m_pDevice->GetDeviceInfo().IsGLDevice() ?
                                      nullptr :
                                      m_pSwapChain->GetCurrentBackBufferRTV();

        auto fb_it = m_FramebufferCache.find(pCurrentBackBufferRTV);
        if (fb_it != m_FramebufferCache.end())
        {
                return fb_it->second;
        }
        else
        {
                auto it = m_FramebufferCache.emplace(pCurrentBackBufferRTV, CreateFramebuffer(pCurrentBackBufferRTV));
                VERIFY_EXPR(it.second);
                return it.first->second;
        }
}
void PBRRenderer::Draw()
{
        const auto& SCDesc = m_pSwapChain->GetDesc();

        {
                // Update constant buffer
                MapHelper<ShaderConstants> Constants(m_pImmediateContext, m_pShaderConstantsCB, MAP_WRITE, MAP_FLAG_DISCARD);

                Constants->ViewProjMatrix = m_ViewProjMatrix;
                // Constants->ViewProjInvMatrix = glm::inverse(m_ViewProjMatrix);
                // Constants->ViewportSize = float4{
                //         static_cast<float>(SCDesc.Width),
                //         static_cast<float>(SCDesc.Height),
                //         1.f / static_cast<float>(SCDesc.Width),
                //         1.f / static_cast<float>(SCDesc.Height) //
                // };
                // Constants->ShowLightVolumes = m_ShowLightVolumes ? 1 : 0;
        }
        {
                MapHelper<ModelBuffer> Constants(m_pImmediateContext, m_pModelBufferCB, MAP_WRITE, MAP_FLAG_DISCARD);

                Constants->ModelTransform = glm::mat4(1.f);
        }

        auto* pFramebuffer = GetCurrentFramebuffer();

        BeginRenderPassAttribs RPBeginInfo;
        RPBeginInfo.pRenderPass = m_pRenderPass;
        RPBeginInfo.pFramebuffer = pFramebuffer;

        OptimizedClearValue ClearValues[5];
        // Color
        ClearValues[0].Color[0] = 0.f;
        ClearValues[0].Color[1] = 0.f;
        ClearValues[0].Color[2] = 0.f;
        ClearValues[0].Color[3] = 0.f;

        // Normal
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




        // Bind vertex and index buffers
        Uint32 offset = 0;
        IBuffer* pBuffs[] = {m_CubeVertexBuffer};
        // Note that RESOURCE_STATE_TRANSITION_MODE_TRANSITION are not allowed inside render pass!
        m_pImmediateContext->SetVertexBuffers(0, 1, pBuffs, &offset, RESOURCE_STATE_TRANSITION_MODE_VERIFY, SET_VERTEX_BUFFERS_FLAG_RESET);
        m_pImmediateContext->SetIndexBuffer(m_CubeIndexBuffer, 0, RESOURCE_STATE_TRANSITION_MODE_VERIFY);
        // Set the cube's pipeline state
        m_pImmediateContext->SetPipelineState(m_pCubePSO);

        // Commit the cube shader's resources
        m_pImmediateContext->CommitShaderResources(m_pCubeSRB, RESOURCE_STATE_TRANSITION_MODE_VERIFY);

        // Draw the grid
        DrawIndexedAttribs DrawAttrs;
        DrawAttrs.IndexType = VT_UINT32; // Index type
        DrawAttrs.NumIndices = 36;
        DrawAttrs.NumInstances = 7;
        DrawAttrs.Flags = DRAW_FLAG_VERIFY_ALL;    // Verify the state of vertex and index buffers
        m_pImmediateContext->DrawIndexed(DrawAttrs);




        {
                MapHelper<ModelBuffer> Constants(m_pImmediateContext, m_pModelBufferCB, MAP_WRITE, MAP_FLAG_DISCARD);
                Constants->ModelTransform = glm::translate(glm::vec3(0.f, 0.f, 5.f));
        }

        m_pImmediateContext->CommitShaderResources(m_pCubeSRB2, RESOURCE_STATE_TRANSITION_MODE_VERIFY);

        // Draw the grid
        DrawAttrs;
        DrawAttrs.IndexType = VT_UINT32; // Index type
        DrawAttrs.NumIndices = 36;
        DrawAttrs.NumInstances = 7 * 7;
        DrawAttrs.Flags = DRAW_FLAG_VERIFY_ALL;    // Verify the state of vertex and index buffers
        m_pImmediateContext->DrawIndexed(DrawAttrs);




        m_pImmediateContext->NextSubpass();


        // Set the lighting PSO
        m_pImmediateContext->SetPipelineState(m_pAmbientLightPSO);

        // Commit shader resources
        m_pImmediateContext->CommitShaderResources(m_pAmbientLightSRB, RESOURCE_STATE_TRANSITION_MODE_VERIFY);

        {
                // Draw quad
                DrawAttribs DrawAttrs;
                DrawAttrs.NumVertices = 4;
                DrawAttrs.Flags = DRAW_FLAG_VERIFY_ALL; // Verify the state of vertex and index buffers
                m_pImmediateContext->Draw(DrawAttrs);
        }


        m_pImmediateContext->EndRenderPass();

        if (m_pDevice->GetDeviceInfo().IsGLDevice())
        {
                // In OpenGL we now have to copy our off-screen buffer to the default framebuffer
                auto* pOffscreenRenderTarget = pFramebuffer->GetDesc().ppAttachments[3]->GetTexture();
                auto* pBackBuffer = m_pSwapChain->GetCurrentBackBufferRTV()->GetTexture();

                CopyTextureAttribs CopyAttribs{pOffscreenRenderTarget, RESOURCE_STATE_TRANSITION_MODE_TRANSITION,
                                               pBackBuffer, RESOURCE_STATE_TRANSITION_MODE_TRANSITION};
                m_pImmediateContext->CopyTexture(CopyAttribs);
        }
}
void PBRRenderer::CreateAmbientLightPSO(Diligent::IShaderSourceInputStreamFactory* pShaderSourceFactory)
{
        GraphicsPipelineStateCreateInfo PSOCreateInfo;
        PipelineStateDesc&              PSODesc = PSOCreateInfo.PSODesc;

        PSODesc.Name = "Ambient light PSO";
        PSODesc.PipelineType = Diligent::PIPELINE_TYPE_GRAPHICS;

        PSOCreateInfo.GraphicsPipeline.pRenderPass = m_pRenderPass;
        PSOCreateInfo.GraphicsPipeline.SubpassIndex = 1; // This PSO will be used within the second subpass

        PSOCreateInfo.GraphicsPipeline.PrimitiveTopology = PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
        PSOCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode = CULL_MODE_NONE;
        PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = False; // Disable depth

        ShaderCreateInfo ShaderCI;
        ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;

        ShaderCI.UseCombinedTextureSamplers = true;

        ShaderCI.pShaderSourceStreamFactory = pShaderSourceFactory;
        // Create a vertex shader
        RefCntAutoPtr<IShader> pVS;
        {
                ShaderCI.Desc.ShaderType = SHADER_TYPE_VERTEX;
                ShaderCI.EntryPoint = "main";
                ShaderCI.Desc.Name = "Ambient light VS";
                ShaderCI.FilePath = "ambient_light.vsh";
                m_pDevice->CreateShader(ShaderCI, &pVS);
                VERIFY_EXPR(pVS != nullptr);
        }

        const auto IsVulkan = m_pDevice->GetDeviceInfo().IsVulkanDevice();

        // Create a pixel shader
        RefCntAutoPtr<IShader> pPS;
        {
                ShaderCI.SourceLanguage = IsVulkan ? SHADER_SOURCE_LANGUAGE_GLSL : SHADER_SOURCE_LANGUAGE_HLSL;
                ShaderCI.Desc.ShaderType = SHADER_TYPE_PIXEL;
                ShaderCI.EntryPoint = "main";
                ShaderCI.Desc.Name = "Ambient light PS";
                ShaderCI.FilePath = IsVulkan ? "ambient_light_glsl.psh" : "ambient_light_hlsl.psh";
                m_pDevice->CreateShader(ShaderCI, &pPS);
                VERIFY_EXPR(pPS != nullptr);
        }

        PSOCreateInfo.pVS = pVS;
        PSOCreateInfo.pPS = pPS;

        PSODesc.ResourceLayout.DefaultVariableType = SHADER_RESOURCE_VARIABLE_TYPE_STATIC;

        // clang-format off
        ShaderResourceVariableDesc Vars[] =
        {
                {SHADER_TYPE_PIXEL, "g_SubpassInputColor", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                {SHADER_TYPE_PIXEL, "g_SubpassInputNormal", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                {SHADER_TYPE_PIXEL, "g_SubpassInputDepthZ", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE}
        };
        // clang-format on
        PSODesc.ResourceLayout.Variables = Vars;
        PSODesc.ResourceLayout.NumVariables = _countof(Vars);

        m_pDevice->CreateGraphicsPipelineState(PSOCreateInfo, &m_pAmbientLightPSO);
        VERIFY_EXPR(m_pAmbientLightPSO != nullptr);

        LD_LOG_INFO("Ambient light pso created");
}
void PBRRenderer::ReleaseWindowResources()
{
        m_FramebufferCache.clear();
        // m_pLightVolumeSRB.Release();
        m_pAmbientLightSRB.Release();
}
}