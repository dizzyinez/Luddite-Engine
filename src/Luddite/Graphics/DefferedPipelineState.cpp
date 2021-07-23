#include "Luddite/Graphics/DefferedPipelineState.hpp"
#include "Luddite/Graphics/Renderer.hpp"
#include "Graphics/GraphicsTools/interface/GraphicsUtilities.h"

using namespace Diligent;
namespace Luddite
{
void DefferedPipelineState::Initialize(
        Diligent::RefCntAutoPtr<Diligent::IRenderPass> pRenderPass,
        const std::string& VSFilePath,
        const std::string& PSFilePath,
        const std::string& PipelineName,
        ShaderAttributeListDescription ConstantShaderAttributes,
        ShaderAttributeListDescription MaterialShaderAttributes,
        ShaderAttributeListDescription ModelShaderAttributes
        )
{
        auto& pDevice = Renderer::GetDevice();
        m_pMaterialLibrary = std::make_unique<MaterialLibrary>(MaterialShaderAttributes);
        m_ConstantShaderAttributes = ConstantShaderAttributes;
        m_ModelShaderAttributes = ModelShaderAttributes;

        GraphicsPipelineStateCreateInfo PSOCreateInfo;
        PipelineStateDesc&              PSODesc = PSOCreateInfo.PSODesc;

        // Pipeline state name is used by the engine to report issues.
        PSODesc.Name = PipelineName.c_str();
        PSODesc.PipelineType = Diligent::PIPELINE_TYPE_GRAPHICS;

        PSOCreateInfo.GraphicsPipeline.pRenderPass = pRenderPass;
        PSOCreateInfo.GraphicsPipeline.SubpassIndex = 0; // We will assume the pipeline only renders on the first pass for now

        PSOCreateInfo.GraphicsPipeline.PrimitiveTopology = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        PSOCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode = CULL_MODE_BACK;
        PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = True;

        ShaderCreateInfo ShaderCI;
        ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;

        ShaderCI.UseCombinedTextureSamplers = true;

        ShaderCI.pShaderSourceStreamFactory = Renderer::GetShaderSourceFactory();
        // Create vertex shader
        RefCntAutoPtr<IShader> pVS;
        {
                ShaderCI.Desc.ShaderType = SHADER_TYPE_VERTEX;
                ShaderCI.EntryPoint = "main";
                ShaderCI.Desc.Name = "Deffered VS";
                ShaderCI.FilePath = VSFilePath.c_str();
                Renderer::GetDevice()->CreateShader(ShaderCI, &pVS);
                VERIFY_EXPR(pVS != nullptr);
        }

        // Create cube pixel shader
        RefCntAutoPtr<IShader> pPS;
        {
                ShaderCI.Desc.ShaderType = SHADER_TYPE_PIXEL;
                ShaderCI.EntryPoint = "main";
                ShaderCI.Desc.Name = "Deffered PS";
                ShaderCI.FilePath = PSFilePath.c_str();
                Renderer::GetDevice()->CreateShader(ShaderCI, &pPS);
                VERIFY_EXPR(pPS != nullptr);
        }

        PSOCreateInfo.pVS = pVS;
        PSOCreateInfo.pPS = pPS;

        PSODesc.ResourceLayout.DefaultVariableType = SHADER_RESOURCE_VARIABLE_TYPE_STATIC;

        const LayoutElement LayoutElems[] =
        {
                LayoutElement{0, 0, 3, VT_FLOAT32, False}, // Attribute 0 - Vertex position
                LayoutElement{1, 0, 3, VT_FLOAT32, False}, // Attribute 1 - Normal direction
                LayoutElement{2, 0, 2, VT_FLOAT32, False} // Attribute 2 - Texture coordinates
        };

        PSOCreateInfo.GraphicsPipeline.InputLayout.LayoutElements = LayoutElems;
        PSOCreateInfo.GraphicsPipeline.InputLayout.NumElements = _countof(LayoutElems);

        ShaderResourceVariableDesc Vars[] =
        {
                {SHADER_TYPE_VERTEX, "ModelConstants", SHADER_RESOURCE_VARIABLE_TYPE_STATIC},
                {SHADER_TYPE_VERTEX, "ShaderConstants", SHADER_RESOURCE_VARIABLE_TYPE_STATIC},
                {SHADER_TYPE_PIXEL, "MaterialConstants", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE}
        };

        PSODesc.ResourceLayout.Variables = Vars;
        PSODesc.ResourceLayout.NumVariables = _countof(Vars);

        // Define immutable sampler for g_Texture.
        SamplerDesc SamLinearClampDesc
        {
                FILTER_TYPE_LINEAR, FILTER_TYPE_LINEAR, FILTER_TYPE_LINEAR,
                TEXTURE_ADDRESS_CLAMP, TEXTURE_ADDRESS_CLAMP, TEXTURE_ADDRESS_CLAMP
        };
        // ImmutableSamplerDesc ImtblSamplers[] =
        // {
        //         // {SHADER_TYPE_PIXEL, "g_Texture", SamLinearClampDesc}
        // };

        // PSODesc.ResourceLayout.ImmutableSamplers = ImtblSamplers;
        // PSODesc.ResourceLayout.NumImmutableSamplers = _countof(ImtblSamplers);

        Renderer::GetDevice()->CreateGraphicsPipelineState(PSOCreateInfo, &m_pPSO);
        VERIFY_EXPR(m_pPSO != nullptr);

        m_ConstantShaderData = m_ConstantShaderAttributes.CreateData();
        m_ModelShaderData = m_ModelShaderAttributes.CreateData();

        CreateUniformBuffer(Renderer::GetDevice(), m_ConstantShaderAttributes.GetSize(), "ShaderConstants", &m_pShaderConstantsBuffer);
        CreateUniformBuffer(Renderer::GetDevice(), m_ModelShaderAttributes.GetSize(), "ModelConstants", &m_pShaderModelBuffer);

        StateTransitionDesc Barriers[] =
        {
                {m_pShaderConstantsBuffer, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_CONSTANT_BUFFER, true},
                {m_pShaderModelBuffer, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_CONSTANT_BUFFER, true}
        };
        Renderer::GetContext()->TransitionResourceStates(_countof(Barriers), Barriers);

        m_pPSO->GetStaticVariableByName(SHADER_TYPE_VERTEX, "ShaderConstants")->Set(m_pShaderConstantsBuffer);
        m_pPSO->GetStaticVariableByName(SHADER_TYPE_VERTEX, "ModelConstants")->Set(m_pShaderModelBuffer);
}
void DefferedPipelineState::PrepareDraw()
{
        Renderer::GetContext()->SetPipelineState(m_pPSO);
        m_ConstantShaderAttributes.MapBuffer(m_ConstantShaderData, m_pShaderConstantsBuffer);
}

void DefferedPipelineState::UploadModelData()
{
        m_ModelShaderAttributes.MapBuffer(m_ModelShaderData, m_pShaderModelBuffer);
}
void DefferedPipelineState::SetMaterial(MaterialHandle Material)
{
        m_pCurrentMaterial = Material;
        m_pMaterialLibrary->m_MaterialShaderAttributes.MapBuffer(Material->m_data, Material->m_pMaterialConstantsBuffer);
        Renderer::GetContext()->CommitShaderResources(Material->m_pMaterialShaderResourceBinding, RESOURCE_STATE_TRANSITION_MODE_VERIFY);


        // Material->Diligent::MAP_WRITE
}
MaterialHandle DefferedPipelineState::GetMaterial(const std::string& Name)
{
        MaterialHandle material = m_pMaterialLibrary->Get(Name);

        if (material->m_pMaterialConstantsBuffer == nullptr)
        {
                CreateUniformBuffer(Renderer::GetDevice(), m_pMaterialLibrary->m_MaterialShaderAttributes.GetSize(), "MaterialConstants", &material->m_pMaterialConstantsBuffer);
        }
        if (material->m_pMaterialShaderResourceBinding == nullptr)
        {
                m_pPSO->CreateShaderResourceBinding(&material->m_pMaterialShaderResourceBinding, true);
        }
        StateTransitionDesc Barriers[] = //
        {
                {material->m_pMaterialConstantsBuffer, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_CONSTANT_BUFFER, true},
        };
        Renderer::GetContext()->TransitionResourceStates(_countof(Barriers), Barriers);

        material->m_pMaterialShaderResourceBinding->GetVariableByName(SHADER_TYPE_PIXEL, "MaterialConstants")->Set(material->m_pMaterialConstantsBuffer);
        return material;
}
}