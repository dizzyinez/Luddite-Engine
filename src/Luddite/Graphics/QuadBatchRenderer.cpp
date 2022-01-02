#include "Luddite/Graphics/QuadBatchRenderer.hpp"
#include "Graphics/GraphicsTools/interface/GraphicsUtilities.h"
#include "Graphics/GraphicsTools/interface/MapHelper.hpp"

using namespace Diligent;

namespace Luddite
{
void QuadBatchRenderer::Initialize(Diligent::RefCntAutoPtr<Diligent::IRenderDevice> pDevice,
                                   Diligent::RefCntAutoPtr<Diligent::IDeviceContext> pImmediateContext,
                                   Diligent::TEXTURE_FORMAT RTVFormat,
                                   Diligent::TEXTURE_FORMAT DSVFormat,
                                   Diligent::RefCntAutoPtr<Diligent::IShaderSourceInputStreamFactory> pShaderSourceFactory,
                                   const std::string& VSFilePath,
                                   const std::string& PSFilePath)
{
        m_pDevice = pDevice;
        m_pImmediateContext = pImmediateContext;

        CreatePipelineState(RTVFormat, DSVFormat, pShaderSourceFactory, VSFilePath, PSFilePath);

        m_QuadVertexBuffer = CreateVertexBuffer();
        m_QuadIndexBuffer = CreateIndexBuffer();

        CreateInstanceBuffer();
        //LoadTextures();
        LD_LOG_INFO("Quad Batch Renderer Initialized");
}
void QuadBatchRenderer::StartBatch()
{
        instance_data_iterator = instance_data.begin();
        texture_data_iterator = texture_data.begin();
}
void QuadBatchRenderer::UploadBatch()
{
        Uint32 data_size = static_cast<Uint32>(sizeof(instance_data[0]) * (instance_data_iterator - instance_data.begin()));
        m_pImmediateContext->UpdateBuffer(m_InstanceBuffer, 0, data_size, instance_data.data(), RESOURCE_STATE_TRANSITION_MODE_TRANSITION);


        // Load a texture array
        RefCntAutoPtr<ITexture> pTexArray;
        int NumTextures = texture_data_iterator - texture_data.begin();


        for (auto tex = texture_data.begin(); tex != texture_data_iterator; tex++)
        {
                const auto& TexDesc = (*tex)->GetDesc();
                if (pTexArray == nullptr)
                {
                        //	Create texture array
                        auto TexArrDesc = TexDesc;
                        TexArrDesc.ArraySize = NumTextures;
                        TexArrDesc.Type = RESOURCE_DIM_TEX_2D_ARRAY;
                        TexArrDesc.Usage = USAGE_DEFAULT;
                        TexArrDesc.BindFlags = BIND_SHADER_RESOURCE;
                        m_pDevice->CreateTexture(TexArrDesc, nullptr, &pTexArray);
                }
        }

        for (int tex = 0; tex < NumTextures; ++tex)
        {
                // // Load current texture
                // std::stringstream FileNameSS;
                // FileNameSS << "DGLogo" << tex << ".png";
                // auto FileName = FileNameSS.str();
                // RefCntAutoPtr<ITexture> SrcTex = TexturedCube::LoadTexture(m_pDevice, FileName.c_str());
                // const auto&             TexDesc = SrcTex->GetDesc();
                // if (pTexArray == nullptr)
                // {
                //         //	Create texture array
                //         auto TexArrDesc = TexDesc;
                //         TexArrDesc.ArraySize = NumTextures;
                //         TexArrDesc.Type = RESOURCE_DIM_TEX_2D_ARRAY;
                //         TexArrDesc.Usage = USAGE_DEFAULT;
                //         TexArrDesc.BindFlags = BIND_SHADER_RESOURCE;
                //         m_pDevice->CreateTexture(TexArrDesc, nullptr, &pTexArray);
                // }
                // // Copy current texture into the texture array
                // for (Uint32 mip = 0; mip < TexDesc.MipLevels; ++mip)
                // {
                //         CopyTextureAttribs CopyAttribs(SrcTex, RESOURCE_STATE_TRANSITION_MODE_TRANSITION,
                //                                        pTexArray, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
                //         CopyAttribs.SrcMipLevel = mip;
                //         CopyAttribs.DstMipLevel = mip;
                //         CopyAttribs.DstSlice = tex;
                //         m_pImmediateContext->CopyTexture(CopyAttribs);
                // }
        }
}
void QuadBatchRenderer::DrawBatch()
{
        instance_data_iterator = instance_data.begin();

        {
                // Map the buffer and write current world-view-projection matrix
                Diligent::MapHelper<glm::mat4x4> CBConstants(m_pImmediateContext, m_VSConstants, MAP_WRITE, MAP_FLAG_DISCARD);
                CBConstants[0] = m_ViewProjMatrix;//.Transpose();
        }

        // Bind vertex, instance and index buffers
        Uint64 offsets[] = {0, 0};
        IBuffer* pBuffs[] = {m_QuadVertexBuffer, m_InstanceBuffer};
        m_pImmediateContext->SetVertexBuffers(0, _countof(pBuffs), pBuffs, offsets, RESOURCE_STATE_TRANSITION_MODE_TRANSITION, SET_VERTEX_BUFFERS_FLAG_RESET);
        m_pImmediateContext->SetIndexBuffer(m_QuadIndexBuffer, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        // Set the pipeline state
        m_pImmediateContext->SetPipelineState(m_pPSO);
        // Commit shader resources. RESOURCE_STATE_TRANSITION_MODE_TRANSITION mode
        // makes sure that resources are transitioned to required states.
        m_pImmediateContext->CommitShaderResources(m_SRB, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        DrawIndexedAttribs DrawAttrs;   // This is an indexed draw call
        DrawAttrs.IndexType = VT_UINT32; // Index type
        DrawAttrs.NumIndices = 6;
        DrawAttrs.NumInstances = instance_data_iterator - instance_data.begin(); // The number of instances
        // Verify the state of vertex and index buffers
        DrawAttrs.Flags = DRAW_FLAG_VERIFY_ALL;
        m_pImmediateContext->DrawIndexed(DrawAttrs);
}
void QuadBatchRenderer::AddQuad(const Texture& texture, const glm::mat4& transform)
{
        instance_data_iterator++;
        if (instance_data_iterator == instance_data.end())
        {
                instance_data_iterator--;
                UploadBatch();
                DrawBatch();
                StartBatch();
        }
        int texture_index = -1;
        for (auto tex = texture_data.begin(); tex != texture_data_iterator; tex++)
        {
                if (texture.GetTexture() == *tex)
                {
                        texture_index = tex - texture_data.begin();
                        break;
                }
        }
        if (texture_index == -1)
        {
                texture_data_iterator++;
                if (texture_data_iterator != texture_data.end())
                {
                        *texture_data_iterator = texture.GetTexture();
                        texture_index = texture_data_iterator - texture_data.begin();
                }
                else
                {
                        instance_data_iterator--;
                        UploadBatch();
                        DrawBatch();
                        StartBatch();
                        *texture_data_iterator = texture.GetTexture();
                        texture_index = 0;
                }
        }
        instance_data_iterator->Matrix = transform;
        instance_data_iterator->TextureIndex = static_cast<float>(texture_index);
}
void QuadBatchRenderer::CreatePipelineState(
        Diligent::TEXTURE_FORMAT RTVFormat,
        Diligent::TEXTURE_FORMAT DSVFormat,
        Diligent::RefCntAutoPtr<Diligent::IShaderSourceInputStreamFactory> pShaderSourceFactory,
        const std::string& VSFilePath,
        const std::string& PSFilePath
        )
{
        LayoutElement LayoutElements[] =
        {
                // Per-vertex data - first buffer slot
                // Attribute 0 - vertex position
                LayoutElement{0, 0, 3, VT_FLOAT32, False},
                // Attribute 1 - texture coordinates
                LayoutElement{1, 0, 2, VT_FLOAT32, False},

                // Per-instance data - second buffer slot
                // We will use four attributes to encode instance-specific 4x4 transformation matrix
                // Attribute 2 - first row
                LayoutElement{2, 1, 4, VT_FLOAT32, False, INPUT_ELEMENT_FREQUENCY_PER_INSTANCE},
                // Attribute 3 - second row
                LayoutElement{3, 1, 4, VT_FLOAT32, False, INPUT_ELEMENT_FREQUENCY_PER_INSTANCE},
                // Attribute 4 - third row
                LayoutElement{4, 1, 4, VT_FLOAT32, False, INPUT_ELEMENT_FREQUENCY_PER_INSTANCE},
                // Attribute 5 - fourth row
                LayoutElement{5, 1, 4, VT_FLOAT32, False, INPUT_ELEMENT_FREQUENCY_PER_INSTANCE},
                // Attribute 6 - texture array index
                LayoutElement{6, 1, 1, VT_FLOAT32, False, INPUT_ELEMENT_FREQUENCY_PER_INSTANCE},
        };

        Diligent::GraphicsPipelineStateCreateInfo PSOCreateInfo;
        Diligent::PipelineStateDesc&              PSODesc = PSOCreateInfo.PSODesc;

        // Pipeline state name is used by the engine to report issues
        // It is always a good idea to give objects descriptive names
        PSODesc.Name = "Quad PSO";

        // This is a graphics pipeline
        PSODesc.PipelineType = Diligent::PIPELINE_TYPE_GRAPHICS;

        PSOCreateInfo.GraphicsPipeline.NumRenderTargets = 1;
        PSOCreateInfo.GraphicsPipeline.RTVFormats[0] = RTVFormat;
        PSOCreateInfo.GraphicsPipeline.DSVFormat = DSVFormat;
        PSOCreateInfo.GraphicsPipeline.PrimitiveTopology = Diligent::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = Diligent::True;
        PSOCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode = Diligent::CULL_MODE_NONE;
        PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = True;

        ShaderCreateInfo ShaderCI;
        ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;
        ShaderCI.UseCombinedTextureSamplers = true;
        ShaderCI.pShaderSourceStreamFactory = pShaderSourceFactory;

        //create vertex shader
        RefCntAutoPtr<IShader> pVS;
        {
                ShaderCI.Desc.ShaderType = SHADER_TYPE_VERTEX;
                ShaderCI.EntryPoint = "main";
                ShaderCI.Desc.Name = "Quad VS";
                ShaderCI.FilePath = VSFilePath.c_str();
                m_pDevice->CreateShader(ShaderCI, &pVS);
        }

        //create pixel shader
        RefCntAutoPtr<IShader> pPS;
        {
                ShaderCI.Desc.ShaderType = SHADER_TYPE_PIXEL;
                ShaderCI.EntryPoint = "main";
                ShaderCI.Desc.Name = "Quad PS";
                ShaderCI.FilePath = PSFilePath.c_str();
                m_pDevice->CreateShader(ShaderCI, &pPS);
        }

        PSOCreateInfo.pVS = pVS;
        PSOCreateInfo.pPS = pPS;

        PSOCreateInfo.GraphicsPipeline.InputLayout.LayoutElements = LayoutElements;
        PSOCreateInfo.GraphicsPipeline.InputLayout.NumElements = _countof(LayoutElements);

        // Define variable type that will be used by default
        PSOCreateInfo.PSODesc.ResourceLayout.DefaultVariableType = SHADER_RESOURCE_VARIABLE_TYPE_STATIC;

        ShaderResourceVariableDesc Vars[] =
        {
                {SHADER_TYPE_PIXEL, "g_Texture", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE}
        };

        PSOCreateInfo.PSODesc.ResourceLayout.Variables = Vars;
        PSOCreateInfo.PSODesc.ResourceLayout.NumVariables = _countof(Vars);

        SamplerDesc SamLinearClampDesc
        {
                FILTER_TYPE_LINEAR, FILTER_TYPE_LINEAR, FILTER_TYPE_LINEAR,
                TEXTURE_ADDRESS_CLAMP, TEXTURE_ADDRESS_CLAMP, TEXTURE_ADDRESS_CLAMP
        };
        ImmutableSamplerDesc ImtblSamplers[] =
        {
                {SHADER_TYPE_PIXEL, "g_Texture", SamLinearClampDesc}
        };

        PSOCreateInfo.PSODesc.ResourceLayout.ImmutableSamplers = ImtblSamplers;
        PSOCreateInfo.PSODesc.ResourceLayout.NumImmutableSamplers = _countof(ImtblSamplers);

        m_pDevice->CreateGraphicsPipelineState(PSOCreateInfo, &m_pPSO);


        // Create dynamic uniform buffer that will store our transformation matrix
        // Dynamic buffers can be frequently updated by the CPU
        CreateUniformBuffer(m_pDevice, sizeof(glm::mat4), "VS constants CB", &m_VSConstants);

        // Since we did not explcitly specify the type for 'Constants' variable, default
        // type (SHADER_RESOURCE_VARIABLE_TYPE_STATIC) will be used. Static variables
        // never change and are bound directly to the pipeline state object.
        m_pPSO->GetStaticVariableByName(SHADER_TYPE_VERTEX, "Constants")->Set(m_VSConstants);

        // Since we are using mutable variable, we must create a shader resource binding object
        // http://diligentgraphics.com/2016/03/23/resource-binding-model-in-diligent-engine-2-0/
        m_pPSO->CreateShaderResourceBinding(&m_SRB, true);
}

void QuadBatchRenderer::CreateTextureAtlas()
{
        DynamicTextureAtlasCreateInfo DTACreateInfo;
        DTACreateInfo.Desc.Name = "Quad Batch Texture Atlas";
        DTACreateInfo.Desc.Type = RESOURCE_DIM_TEX_2D_ARRAY;
        // VertBuffDesc.Usage = USAGE_IMMUTABLE;
        DTACreateInfo.Desc.BindFlags = BIND_VERTEX_BUFFER;
        // DTACreateInfo.Desc.ArraySize

        CreateDynamicTextureAtlas(m_pDevice, DTACreateInfo, &m_pTextureAtlas);
}

RefCntAutoPtr<IBuffer> QuadBatchRenderer::CreateVertexBuffer()
{
        // Layout of this structure matches the one we defined in the pipeline state
        struct Vertex
        {
                glm::vec3 pos;
                glm::vec2 uv;
        };

        // clang-format off
        Vertex QuadVerts[] =
        {
                {glm::vec3(-1, -1, 0), glm::vec2(0, 1)},
                {glm::vec3(-1, +1, 0), glm::vec2(0, 0)},
                {glm::vec3(+1, +1, 0), glm::vec2(1, 0)},
                {glm::vec3(+1, -1, 0), glm::vec2(1, 1)}
        };
        // clang-format on

        BufferDesc VertBuffDesc;
        VertBuffDesc.Name = "Quad vertex buffer";
        VertBuffDesc.Usage = USAGE_IMMUTABLE;
        VertBuffDesc.BindFlags = BIND_VERTEX_BUFFER;
        VertBuffDesc.Size = sizeof(QuadVerts);
        BufferData VBData;
        VBData.pData = QuadVerts;
        VBData.DataSize = sizeof(QuadVerts);
        RefCntAutoPtr<IBuffer> pQuadVertexBuffer;

        m_pDevice->CreateBuffer(VertBuffDesc, &VBData, &pQuadVertexBuffer);

        return pQuadVertexBuffer;
}

RefCntAutoPtr<IBuffer> QuadBatchRenderer::CreateIndexBuffer()
{
        // clang-format off
        Uint32 Indices[] =
        {
                2, 0, 1, 2, 3, 0
        };
        // clang-format on

        BufferDesc IndBuffDesc;
        IndBuffDesc.Name = "Quad index buffer";
        IndBuffDesc.Usage = USAGE_IMMUTABLE;
        IndBuffDesc.BindFlags = BIND_INDEX_BUFFER;
        IndBuffDesc.Size = sizeof(Indices);
        BufferData IBData;
        IBData.pData = Indices;
        IBData.DataSize = sizeof(Indices);
        RefCntAutoPtr<IBuffer> pBuffer;
        m_pDevice->CreateBuffer(IndBuffDesc, &IBData, &pBuffer);
        return pBuffer;
}

void QuadBatchRenderer::CreateInstanceBuffer()
{
        // Create instance data buffer that will store transformation matrices
        BufferDesc InstBuffDesc;
        InstBuffDesc.Name = "Instance data buffer";
        // Use default usage as this buffer will only be updated when grid size changes
        InstBuffDesc.Usage = USAGE_DEFAULT; //TODO: CHANGE THIS
        InstBuffDesc.BindFlags = BIND_VERTEX_BUFFER;
        InstBuffDesc.Size = sizeof(InstanceData) * MaxInstances;
        m_pDevice->CreateBuffer(InstBuffDesc, nullptr, &m_InstanceBuffer);
}
}
