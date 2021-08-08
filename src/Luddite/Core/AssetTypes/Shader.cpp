#include "Luddite/Core/AssetTypes/Shader.hpp"
#include "Luddite/Graphics/Renderer.hpp"
#include "yaml-cpp/yaml.h"

using namespace Diligent;

namespace Luddite
{
void ShaderLibrary::Initialize()
{
        m_AssetBaseDir = "./Assets/Shaders/";
        m_Extensions.push_back(L".shader");
}

Shader* ShaderLibrary::LoadFromFile(const std::filesystem::path& path)
{
        Shader* pShader = new Shader();
        YAML::Node yaml = YAML::LoadFile(path.string());
        ShaderCreateInfo ShaderCI;
        ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;
        ShaderCI.UseCombinedTextureSamplers = true;
        ShaderCI.pShaderSourceStreamFactory = Renderer::GetShaderSourceFactory();
        std::string name = yaml["Name"].as<std::string>();
        std::string src = yaml["Shader"].as<std::string>();
        ShaderCI.Source = src.c_str();

        //Create the vertex shader
        {
                ShaderCI.Desc.ShaderType = SHADER_TYPE_VERTEX;
                std::stringstream shader_name(name);
                shader_name << " Vertex Shader";
                ShaderCI.Desc.Name = shader_name.str().c_str();
                ShaderCI.EntryPoint = yaml["Entry Points"]["Vertex"].as<std::string>().c_str();
                Renderer::GetDevice()->CreateShader(ShaderCI, &pShader->m_pVertexShader);
        }
        //Verify the vertex shader compiled
        if (!pShader->m_pVertexShader)
        {
                LD_LOG_ERROR("Failed to compile Vertex shader for: {} ", name);
                delete pShader;
                return nullptr;
        }
        //Create the pixel shader
        {
                ShaderCI.Desc.ShaderType = SHADER_TYPE_PIXEL;
                std::stringstream shader_name(name);
                shader_name << " Pixel Shader";
                ShaderCI.Desc.Name = shader_name.str().c_str();
                ShaderCI.EntryPoint = yaml["Entry Points"]["Pixel"].as<std::string>().c_str();
                Renderer::GetDevice()->CreateShader(ShaderCI, &pShader->m_pPixelShader);
        }
        //Verify the pixel shader compiled
        if (!pShader->m_pVertexShader)
        {
                LD_LOG_ERROR("Failed to compile Vertex shader for: {} ", name);
                delete pShader;
                return nullptr;
        }


        //Create PSO
        {
                GraphicsPipelineStateCreateInfo PSOCreateInfo;
                PipelineStateDesc& PSODesc = PSOCreateInfo.PSODesc;
                PSODesc.Name = name.c_str();
                PSODesc.PipelineType = Diligent::PIPELINE_TYPE_GRAPHICS;
                PSOCreateInfo.GraphicsPipeline.PrimitiveTopology = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
                PSOCreateInfo.pVS = pShader->m_pVertexShader;
                PSOCreateInfo.pPS = pShader->m_pPixelShader;

                PSOCreateInfo.GraphicsPipeline.NumRenderTargets = 1;
                PSOCreateInfo.GraphicsPipeline.RTVFormats[0] = Renderer::GetDefaultRTVFormat();
                PSOCreateInfo.GraphicsPipeline.DSVFormat = TEX_FORMAT_D32_FLOAT_S8X24_UINT;
                PSODesc.ResourceLayout.DefaultVariableType = SHADER_RESOURCE_VARIABLE_TYPE_STATIC;

                if (yaml["Cull Mode"])
                {
                        auto cull_mode_string = yaml["Cull Mode"].as<std::string>();
                        if (cull_mode_string == "Back")
                                PSOCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode = CULL_MODE_BACK;
                        else if (cull_mode_string == "Front")
                                PSOCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode = CULL_MODE_FRONT;
                        else if (cull_mode_string == "None")
                                PSOCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode = CULL_MODE_NONE;
                }

                if (yaml["Depth Enable"])
                        PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = yaml["Depth Enable"].as<bool>();
                if (yaml["Depth Write Enable"])
                        PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthWriteEnable = yaml["Depth Write Enable"].as<bool>();
                if (yaml["Depth Func"])
                {
                        auto depth_func_string = yaml["Depth Func"].as<std::string>();
                        if (depth_func_string == "Less")
                                PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthFunc = COMPARISON_FUNC_LESS;
                        else if (depth_func_string == "Equal")
                                PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthFunc = COMPARISON_FUNC_EQUAL;
                        else if (depth_func_string == "Greater")
                                PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthFunc = COMPARISON_FUNC_GREATER;
                        else if (depth_func_string == "Less Equal")
                                PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthFunc = COMPARISON_FUNC_LESS_EQUAL;
                        else if (depth_func_string == "Greater Equal")
                                PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthFunc = COMPARISON_FUNC_GREATER_EQUAL;
                        else if (depth_func_string == "Not Equal")
                                PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthFunc = COMPARISON_FUNC_NOT_EQUAL;
                        else if (depth_func_string == "Never")
                                PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthFunc = COMPARISON_FUNC_NEVER;
                        else if (depth_func_string == "Always")
                                PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthFunc = COMPARISON_FUNC_ALWAYS;
                }
                Renderer::GetDevice()->CreateGraphicsPipelineState(PSOCreateInfo, &pShader->m_pPSO);
        }
        if (!pShader->m_pPSO)
        {
                LD_LOG_ERROR("Failed to create PSO for shader: {} ", name);
                delete pShader;
                return nullptr;
        }


        if (pShader->m_pPixelShader)
        {
                uint32_t rcount = pShader->m_pPixelShader->GetResourceCount();
                Diligent::ShaderResourceDesc desc;
                for (uint32_t i = 0; i < rcount; i++)
                {
                        pShader->m_pPixelShader->GetResourceDesc(i, desc);
                        LD_LOG_TRACE("Name : {}, Type:{}, Size: {} ", desc.Name, desc.Type, desc.ArraySize);
                }
        }
        return pShader;
}
}