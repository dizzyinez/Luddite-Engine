#include "Luddite/Graphics/ShaderPipeline.hpp"
#include "Graphics/GraphicsTools/interface/GraphicsUtilities.h"

using namespace Diligent;
namespace Luddite
{
void ShaderPipeline::Initialize(
        Diligent::RefCntAutoPtr<Diligent::IRenderDevice> pDevice,
        Diligent::RefCntAutoPtr<Diligent::IRenderPass> pRenderPass,
        Diligent::RefCntAutoPtr<Diligent::IShaderSourceInputStreamFactory> pShaderSourceFactory,
        const std::string& VSFilePath,
        const std::string& PSFilePath,
        const std::string& PipelineName,
        // Diligent::LayoutElement* ShaderLayoutElements,
        ShaderAttributeListDescription ConstantShaderAttributes,
        ShaderAttributeListDescription MaterialShaderAttributes
        )
{
        m_pMaterialLibrary = std::make_unique<MaterialLibrary>(MaterialShaderAttributes);

        m_pDevice = pDevice;
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

        // OpenGL backend requires emulated combined HLSL texture samplers (g_Texture + g_Texture_sampler combination)
        ShaderCI.UseCombinedTextureSamplers = true;

        ShaderCI.pShaderSourceStreamFactory = pShaderSourceFactory;
        // Create vertex shader
        RefCntAutoPtr<IShader> pVS;
        {
                ShaderCI.Desc.ShaderType = SHADER_TYPE_VERTEX;
                ShaderCI.EntryPoint = "main";
                ShaderCI.Desc.Name = "Cube VS";
                ShaderCI.FilePath = VSFilePath.c_str();
                m_pDevice->CreateShader(ShaderCI, &pVS);
                VERIFY_EXPR(pVS != nullptr);
        }

        // Create cube pixel shader
        RefCntAutoPtr<IShader> pPS;
        {
                ShaderCI.Desc.ShaderType = SHADER_TYPE_PIXEL;
                ShaderCI.EntryPoint = "main";
                ShaderCI.Desc.Name = "Cube PS";
                ShaderCI.FilePath = PSFilePath.c_str();
                m_pDevice->CreateShader(ShaderCI, &pPS);
                VERIFY_EXPR(pPS != nullptr);
        }

        const LayoutElement LayoutElems[] =
        {
                LayoutElement{0, 0, 3, VT_FLOAT32, False}, // Attribute 0 - vertex position
                LayoutElement{1, 0, 3, VT_FLOAT32, False}, // Attribute 1 - Normal direction
                LayoutElement{2, 0, 2, VT_FLOAT32, False} // Attribute 2 - texture coordinates
        };

        PSOCreateInfo.pVS = pVS;
        PSOCreateInfo.pPS = pPS;

        PSOCreateInfo.GraphicsPipeline.InputLayout.LayoutElements = LayoutElems;
        PSOCreateInfo.GraphicsPipeline.InputLayout.NumElements = _countof(LayoutElems);

        // Define variable type that will be used by default
        PSODesc.ResourceLayout.DefaultVariableType = SHADER_RESOURCE_VARIABLE_TYPE_STATIC;
/*
 *      // clang-format off
 *      ShaderResourceVariableDesc Vars[] =
 *      {
 *              // {SHADER_TYPE_PIXEL, "g_Texture", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE}
 *      };
 *      // clang-format on
 *      PSODesc.ResourceLayout.Variables = Vars;
 *      PSODesc.ResourceLayout.NumVariables = _countof(Vars);
 *
 *      // Define immutable sampler for g_Texture.
 *      SamplerDesc SamLinearClampDesc
 *      {
 *              FILTER_TYPE_LINEAR, FILTER_TYPE_LINEAR, FILTER_TYPE_LINEAR,
 *              TEXTURE_ADDRESS_CLAMP, TEXTURE_ADDRESS_CLAMP, TEXTURE_ADDRESS_CLAMP
 *      };
 *      ImmutableSamplerDesc ImtblSamplers[] =
 *      {
 *              {SHADER_TYPE_PIXEL, "g_Texture", SamLinearClampDesc}
 *      };
 *
 *      PSODesc.ResourceLayout.ImmutableSamplers = ImtblSamplers;
 *      PSODesc.ResourceLayout.NumImmutableSamplers = _countof(ImtblSamplers);
 */


        // m_pDevice->CreateGraphicsPipelineState(PSOCreateInfo, &m_pCubePSO);
        // VERIFY_EXPR(m_pCubePSO != nullptr);
}
void ShaderPipeline::PrepareDraw()
{
}
void ShaderPipeline::SetMaterial(MaterialHandle Material)
{
}
MaterialHandle ShaderPipeline::GetMaterial(const std::string& Name)
{
        MaterialHandle material = m_pMaterialLibrary->Get(Name);

        if (material->m_pMaterialConstantsBuffer == nullptr)
        {
                CreateUniformBuffer(m_pDevice, m_pMaterialLibrary->m_MaterialShaderAttributes.GetSize(), "MaterialConstants", &material->m_pMaterialConstantsBuffer);
        }
        return material;
}
}