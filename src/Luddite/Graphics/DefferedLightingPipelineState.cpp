#include "Luddite/Graphics/DefferedLightingPipelineState.hpp"
#include "Luddite/Graphics/Renderer.hpp"

using namespace Diligent;
namespace Luddite
{
void DefferedLightingPipelineState::Initialize(
        Diligent::RefCntAutoPtr<Diligent::IRenderPass> pRenderPass,
        const std::string& VSFilePath,
        const std::string& PSFilePath,
        const std::string& Name
        // ShaderAttributeListDescription ConstantShaderAttributes,
        // ShaderAttributeListDescription MaterialShaderAttributes
        )
{
        GraphicsPipelineStateCreateInfo PSOCreateInfo;
        PipelineStateDesc&              PSODesc = PSOCreateInfo.PSODesc;

        PSODesc.Name = Name.c_str();
        PSODesc.PipelineType = Diligent::PIPELINE_TYPE_GRAPHICS;

        PSOCreateInfo.GraphicsPipeline.pRenderPass = pRenderPass;
        PSOCreateInfo.GraphicsPipeline.SubpassIndex = 1; // This PSO will be used within the second subpass

        PSOCreateInfo.GraphicsPipeline.PrimitiveTopology = PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
        PSOCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode = CULL_MODE_NONE;
        PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = False; // Disable depth

        ShaderCreateInfo ShaderCI;
        ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;

        ShaderCI.UseCombinedTextureSamplers = true;

        ShaderCI.pShaderSourceStreamFactory = Renderer::GetShaderSourceFactory();
        // Create a vertex shader
        RefCntAutoPtr<IShader> pVS;
        {
                ShaderCI.Desc.ShaderType = SHADER_TYPE_VERTEX;
                ShaderCI.EntryPoint = "main";
                ShaderCI.Desc.Name = "light VS";
                ShaderCI.FilePath = VSFilePath.c_str();
                Renderer::GetDevice()->CreateShader(ShaderCI, &pVS);
                VERIFY_EXPR(pVS != nullptr);
        }

        // const auto IsVulkan = m_pDevice->GetDeviceCaps().IsVulkanDevice();

        // Create a pixel shader
        RefCntAutoPtr<IShader> pPS;
        {
                ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;
                ShaderCI.Desc.ShaderType = SHADER_TYPE_PIXEL;
                ShaderCI.EntryPoint = "main";
                ShaderCI.Desc.Name = "light PS";
                ShaderCI.FilePath = PSFilePath.c_str();
                Renderer::GetDevice()->CreateShader(ShaderCI, &pPS);
                VERIFY_EXPR(pPS != nullptr);
        }

        PSOCreateInfo.pVS = pVS;
        PSOCreateInfo.pPS = pPS;

        PSODesc.ResourceLayout.DefaultVariableType = SHADER_RESOURCE_VARIABLE_TYPE_STATIC;

        ShaderResourceVariableDesc Vars[] =
        {
                {SHADER_TYPE_PIXEL, "g_SubpassInputColor", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                {SHADER_TYPE_PIXEL, "g_SubpassInputNormal", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
                {SHADER_TYPE_PIXEL, "g_SubpassInputDepthZ", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE}
        };

        PSODesc.ResourceLayout.Variables = Vars;
        PSODesc.ResourceLayout.NumVariables = _countof(Vars);

        Renderer::GetDevice()->CreateGraphicsPipelineState(PSOCreateInfo, &m_pPSO);
        VERIFY_EXPR(m_pPSO != nullptr);
}
}