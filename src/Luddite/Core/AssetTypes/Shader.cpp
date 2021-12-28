#include "Luddite/Core/AssetTypes/Shader.hpp"
#include "GraphicsTypes.h"
#include "Luddite/Core/AssetTypes/Model.hpp"
#include "Luddite/Core/Profiler.hpp"
#include "Luddite/Graphics/Renderer.hpp"
#include "PipelineResourceSignature.h"
#include "ShaderResourceVariable.h"
#include "yaml-cpp/yaml.h"
#include "Graphics/GraphicsTools/interface/GraphicsUtilities.h"


using namespace Diligent;

namespace Luddite {
void ShaderLibrary::Initialize() {
        m_AssetBaseDir = "./Assets/Shaders/";
        m_Extensions.emplace(L".ldshader");
        // TODO: set default shader :)
}

Shader *ShaderLibrary::LoadFromFile(const std::filesystem::path &path) {
        std::stringstream ss;
        ss << "Loading Shader: " << path;
        LD_PROFILE_SCOPE(ss.str());
        Shader *pShader = new Shader();
        YAML::Node yaml = YAML::LoadFile(path.string());
        ShaderCreateInfo ShaderCI;
        ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;
        ShaderCI.UseCombinedTextureSamplers = true;
        ShaderCI.pShaderSourceStreamFactory = Renderer::GetShaderSourceFactory();
        std::string name = yaml["Name"].as<std::string>();
        pShader->m_Name = name;
        std::string src = yaml["Shader"].as<std::string>();
        ShaderCI.Source = src.c_str();

        // Create the vertex shader
        {
                ShaderCI.Desc.ShaderType = SHADER_TYPE_VERTEX;
                std::stringstream shader_name(name);
                shader_name << " Vertex Shader";
                ShaderCI.Desc.Name = shader_name.str().c_str();
                ShaderCI.EntryPoint =
                        yaml["Entry Points"]["Vertex"].as<std::string>().c_str();
                Renderer::GetDevice()->CreateShader(ShaderCI, &pShader->m_pVertexShader);
        }
        // Verify the vertex shader compiled
        if (!pShader->m_pVertexShader)
        {
                LD_LOG_ERROR("Failed to compile Vertex shader for: {} ", name);
                delete pShader;
                return nullptr;
        }
        // Create the pixel shader
        {
                ShaderCI.Desc.ShaderType = SHADER_TYPE_PIXEL;
                std::stringstream shader_name(name);
                shader_name << " Pixel Shader";
                ShaderCI.Desc.Name = shader_name.str().c_str();
                ShaderCI.EntryPoint =
                        yaml["Entry Points"]["Pixel"].as<std::string>().c_str();
                Renderer::GetDevice()->CreateShader(ShaderCI, &pShader->m_pPixelShader);
        }
        // Verify the pixel shader compiled
        if (!pShader->m_pVertexShader)
        {
                LD_LOG_ERROR("Failed to compile Vertex shader for: {} ", name);
                delete pShader;
                return nullptr;
        }

        // Create PSO
        GraphicsPipelineStateCreateInfo PSOCreateInfo;
        PipelineStateDesc &PSODesc = PSOCreateInfo.PSODesc;
        PSODesc.Name = name.c_str();
        PSODesc.PipelineType = Diligent::PIPELINE_TYPE_GRAPHICS;
        PSOCreateInfo.GraphicsPipeline.PrimitiveTopology =
                PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        PSOCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode = CULL_MODE_NONE;
        PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = True;
        PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthFunc =
                COMPARISON_FUNC_LESS_EQUAL;
        PSOCreateInfo.pVS = pShader->m_pVertexShader;
        PSOCreateInfo.pPS = pShader->m_pPixelShader;

        PSOCreateInfo.GraphicsPipeline.NumRenderTargets = 1;
        PSOCreateInfo.GraphicsPipeline.RTVFormats[0] =
                Renderer::GetDefaultRTVFormat();
        PSOCreateInfo.GraphicsPipeline.DSVFormat =
                Renderer::GetDefaultDSVFormat();
        PSODesc.ResourceLayout.DefaultVariableType =
                SHADER_RESOURCE_VARIABLE_TYPE_STATIC;

        PSOCreateInfo.GraphicsPipeline.InputLayout.LayoutElements =
                VertexLayoutElements;
        PSOCreateInfo.GraphicsPipeline.InputLayout.NumElements =
                _countof(VertexLayoutElements);

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
                PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable =
                        yaml["Depth Enable"].as<bool>();
        if (yaml["Depth Write Enable"])
                PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthWriteEnable =
                        yaml["Depth Write Enable"].as<bool>();
        if (yaml["Depth Func"])
        {
                auto depth_func_string = yaml["Depth Func"].as<std::string>();
                if (depth_func_string == "Less")
                        PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthFunc =
                                COMPARISON_FUNC_LESS;
                else if (depth_func_string == "Equal")
                        PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthFunc =
                                COMPARISON_FUNC_EQUAL;
                else if (depth_func_string == "Greater")
                        PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthFunc =
                                COMPARISON_FUNC_GREATER;
                else if (depth_func_string == "Less Equal")
                        PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthFunc =
                                COMPARISON_FUNC_LESS_EQUAL;
                else if (depth_func_string == "Greater Equal")
                        PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthFunc =
                                COMPARISON_FUNC_GREATER_EQUAL;
                else if (depth_func_string == "Not Equal")
                        PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthFunc =
                                COMPARISON_FUNC_NOT_EQUAL;
                else if (depth_func_string == "Never")
                        PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthFunc =
                                COMPARISON_FUNC_NEVER;
                else if (depth_func_string == "Always")
                        PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthFunc =
                                COMPARISON_FUNC_ALWAYS;
        }

        SamplerDesc SamLinearClampDesc
        {
                FILTER_TYPE_LINEAR, FILTER_TYPE_LINEAR, FILTER_TYPE_LINEAR,
                TEXTURE_ADDRESS_CLAMP, TEXTURE_ADDRESS_CLAMP, TEXTURE_ADDRESS_CLAMP
        };

        std::vector<Diligent::ShaderResourceVariableDesc> vars;
        std::vector<Diligent::ImmutableSamplerDesc> imtbl_samplers;
        if (pShader->m_pVertexShader)
        {
                uint32_t rcount = pShader->m_pVertexShader->GetResourceCount();
                Diligent::ShaderResourceDesc desc;
                for (uint32_t i = 0; i < rcount; i++)
                {
                        pShader->m_pVertexShader->GetResourceDesc(i, desc);
                        switch (desc.Type)
                        {
                        case Diligent::SHADER_RESOURCE_TYPE_CONSTANT_BUFFER:
                        case Diligent::SHADER_RESOURCE_TYPE_BUFFER_SRV:
                        case Diligent::SHADER_RESOURCE_TYPE_BUFFER_UAV:
                                if (static_buffers.find(desc.Name) != static_buffers.end())
                                {
                                        vars.push_back({SHADER_TYPE_VERTEX, desc.Name,
                                                        SHADER_RESOURCE_VARIABLE_TYPE_STATIC});
                                }
                                else if (mutable_buffers.find(desc.Name) != mutable_buffers.end())
                                {
                                        vars.push_back({SHADER_TYPE_VERTEX, desc.Name,
                                                        SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE});
                                        pShader->m_MutableBuffersVertex.emplace_back(desc.Name);
                                }
                                else if (strcmp(desc.Name, "Properties") == 0)
                                        pShader->vertex_shader_uses_properties = true;
                                else
                                {
                                        LD_LOG_ERROR("Unable to find variable: {} for shader {}", desc.Name,
                                                name);
                                        delete pShader;
                                        return nullptr;
                                }
                                break;

                        case Diligent::SHADER_RESOURCE_TYPE_TEXTURE_SRV:
                                if (provided_textures.find(desc.Name) != provided_textures.end())
                                        pShader->m_ProvidedTexturesVertex.emplace_back(desc.Name);
                                pShader->m_PropertiesBufferDescription.AddTextureVertexShader(desc.Name);
                                vars.push_back({SHADER_TYPE_VERTEX, desc.Name, SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE});
                                imtbl_samplers.push_back({SHADER_TYPE_VERTEX, desc.Name, SamLinearClampDesc});
                                break;

                        default:
                                LD_LOG_WARN("Shader {} has variable {} of type {} which is currently unsupported!", name, desc.Name, desc.Type);
                                break;
                        }
                }
        }

        if (pShader->m_pPixelShader)
        {
                uint32_t rcount = pShader->m_pPixelShader->GetResourceCount();
                Diligent::ShaderResourceDesc desc;
                for (uint32_t i = 0; i < rcount; i++)
                {
                        pShader->m_pPixelShader->GetResourceDesc(i, desc);
                        switch (desc.Type)
                        {
                        case Diligent::SHADER_RESOURCE_TYPE_CONSTANT_BUFFER:
                        case Diligent::SHADER_RESOURCE_TYPE_BUFFER_SRV:
                        case Diligent::SHADER_RESOURCE_TYPE_BUFFER_UAV:
                                if (static_buffers.find(desc.Name) != static_buffers.end())
                                {
                                        vars.push_back({SHADER_TYPE_PIXEL, desc.Name,
                                                        SHADER_RESOURCE_VARIABLE_TYPE_STATIC});
                                }
                                else if (mutable_buffers.find(desc.Name) != mutable_buffers.end())
                                {
                                        vars.push_back({SHADER_TYPE_PIXEL, desc.Name,
                                                        SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE});
                                        pShader->m_MutableBuffersPixel.emplace_back(desc.Name);
                                }
                                else if (strcmp(desc.Name, "Properties") == 0)
                                        pShader->pixel_shader_uses_properties = true;
                                else
                                {
                                        LD_LOG_ERROR("Unable to find variable: {} for shader {}", desc.Name,
                                                name);
                                        delete pShader;
                                        return nullptr;
                                }
                                break;

                        case Diligent::SHADER_RESOURCE_TYPE_TEXTURE_SRV:
                                if (provided_textures.find(desc.Name) != provided_textures.end())
                                        pShader->m_ProvidedTexturesPixel.emplace_back(desc.Name);
                                pShader->m_PropertiesBufferDescription.AddTexturePixelShader(desc.Name);
                                vars.push_back({SHADER_TYPE_PIXEL, desc.Name, SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE});
                                imtbl_samplers.push_back({SHADER_TYPE_PIXEL, desc.Name, SamLinearClampDesc});
                                break;

                        default:
                                LD_LOG_WARN("Shader {} has variable {} of type {} which is currently unsupported!", name, desc.Name, desc.Type);
                                break;
                        }
                }
        }

        PSODesc.ResourceLayout.Variables = vars.data();
        PSODesc.ResourceLayout.NumVariables = vars.size();
        PSODesc.ResourceLayout.ImmutableSamplers = imtbl_samplers.data();
        PSODesc.ResourceLayout.NumImmutableSamplers = imtbl_samplers.size();

        Renderer::GetDevice()->CreateGraphicsPipelineState(PSOCreateInfo,
                &pShader->m_pPSO);
        if (!pShader->m_pPSO)
        {
                LD_LOG_ERROR("Failed to create PSO for shader: {} ", name);
                delete pShader;
                return nullptr;
        }
        for (auto &var : vars)
        {
                if (var.Type == SHADER_RESOURCE_VARIABLE_TYPE_STATIC)
                        pShader->m_pPSO->GetStaticVariableByName(var.ShaderStages, var.Name)
                        ->Set(static_buffers[var.Name]);
        }

        // Create ShaderBufferDescription
        if (yaml["Properties"])
        {
                for (auto prop : yaml["Properties"])
                        if (prop.second["type"])
                        {
                                auto type_str = prop.second["type"].as<std::string>();
                                auto type =
                                        pShader->m_PropertiesBufferDescription.GetTypenameFromString(
                                                type_str);
                                if (type != ShaderBufferDescription::ValueType::NONE)
                                        pShader->m_PropertiesBufferDescription.Add(
                                                prop.first.as<std::string>(), type);
                        }
                CreateUniformBuffer(Renderer::GetDevice(),
                        pShader->m_PropertiesBufferDescription.GetSize(),
                        "Properties", &pShader->m_PropertiesBuffer);
                StateTransitionDesc Barriers[] = {{pShader->m_PropertiesBuffer,
                        RESOURCE_STATE_UNKNOWN,
                        RESOURCE_STATE_CONSTANT_BUFFER, true}};
                Renderer::GetContext()->TransitionResourceStates(_countof(Barriers),
                        Barriers);
                if (pShader->vertex_shader_uses_properties)
                        pShader->m_pPSO->GetStaticVariableByName(SHADER_TYPE_VERTEX, "Properties")
                        ->Set(pShader->m_PropertiesBuffer);
                if (pShader->pixel_shader_uses_properties)
                        pShader->m_pPSO->GetStaticVariableByName(SHADER_TYPE_PIXEL, "Properties")
                        ->Set(pShader->m_PropertiesBuffer);
        }

        return pShader;
}
} // namespace Luddite
