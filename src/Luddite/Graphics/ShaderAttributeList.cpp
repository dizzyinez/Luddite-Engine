#include "Luddite/Graphics/ShaderAttributeList.hpp"
#include "Luddite/Graphics/Renderer.hpp"
namespace Luddite
{
void ShaderAttributeListDescription::MapBuffer(ShaderAttributeListData& data, Diligent::RefCntAutoPtr<Diligent::IBuffer> buffer)
{
        if (GetSize() == 0)
                return;
        char* Data;
        Renderer::GetContext()->MapBuffer(buffer, Diligent::MAP_WRITE, Diligent::MAP_FLAG_DISCARD, (Diligent::PVoid&)Data);

        memcpy(Data, data.Data, data.Size);

        Renderer::GetContext()->UnmapBuffer(buffer, Diligent::MAP_WRITE);
}

void ShaderAttributeListDescription::MapTextures(ShaderAttributeListData& data, Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> srb)
{
        for (auto& tex_name : Textures)
        {
                // LD_LOG_TRACE("MAPPING TEXTURE {}", tex_name);
                auto tex = data.GetTexture(tex_name).get()->GetTexture();
                auto var = srb->GetVariableByName(Diligent::SHADER_TYPE_PIXEL, tex_name.c_str());
                var->Set(tex->GetDefaultView(Diligent::TEXTURE_VIEW_SHADER_RESOURCE));
        }
}

ShaderAttributeListData ShaderAttributeListDescription::CreateData()
{
        ShaderAttributeListData new_data;
        new_data.Size = m_Size;
        new_data.Data = (char*)malloc(m_Size);
        // LD_LOG_TRACE("SIZE: {}", m_Size);
        SetDefaultAttribs(new_data);
        return new_data;
}

void ShaderAttributeListDescription::SetDefaultAttribs(ShaderAttributeListData& data)
{
        for (const auto& pair : Attributes)
        {
                const auto& name = pair.first;
                switch (pair.second.type)
                {
                case ShaderAttributeListDescription::ValueType::FLOAT:
                        SetFloat(data, name, 0.f);
                        break;

                case ShaderAttributeListDescription::ValueType::INT:
                        SetInt(data, name, 0);
                        break;

                case ShaderAttributeListDescription::ValueType::UINT:
                        SetUInt(data, name, 0);
                        break;

                case ShaderAttributeListDescription::ValueType::VEC2:
                        SetVec2(data, name, glm::vec2(0.f));
                        break;

                case ShaderAttributeListDescription::ValueType::IVEC2:
                        SetIVec2(data, name, glm::ivec2(0));
                        break;

                case ShaderAttributeListDescription::ValueType::UVEC2:
                        SetUVec2(data, name, glm::uvec2(0));
                        break;

                case ShaderAttributeListDescription::ValueType::VEC3:
                        SetVec3(data, name, glm::vec3(0.f));
                        break;

                case ShaderAttributeListDescription::ValueType::IVEC3:
                        SetIVec3(data, name, glm::ivec3(0));
                        break;

                case ShaderAttributeListDescription::ValueType::UVEC3:
                        SetUVec3(data, name, glm::uvec3(0));
                        break;

                case ShaderAttributeListDescription::ValueType::VEC4:
                        SetVec4(data, name, glm::vec4(0.f));
                        break;

                case ShaderAttributeListDescription::ValueType::IVEC4:
                        SetIVec4(data, name, glm::ivec4(0));
                        break;

                case ShaderAttributeListDescription::ValueType::UVEC4:
                        SetUVec4(data, name, glm::uvec4(0));
                        break;

                case ShaderAttributeListDescription::ValueType::MAT3:
                        SetMat3(data, name, glm::mat3(0.f));
                        break;

                case ShaderAttributeListDescription::ValueType::MAT4:
                        SetMat4(data, name, glm::mat4(0.f));
                        break;
                }
        }
}
}