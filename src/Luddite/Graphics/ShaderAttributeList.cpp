#include "Luddite/Graphics/ShaderAttributeList.hpp"
#include "Luddite/Graphics/Renderer.hpp"
namespace Luddite
{
void ShaderAttributeListDescription::MapBuffer(ShaderAttributeListData& data, Diligent::RefCntAutoPtr<Diligent::IBuffer> buffer)
{
        void* DataVoid = malloc(GetSize());
        uint8_t* Data = (uint8_t*)DataVoid;
        Renderer::GetContext()->MapBuffer(buffer, Diligent::MAP_WRITE, Diligent::MAP_FLAG_DISCARD, (Diligent::PVoid&)Data);

        for (const auto& pair : Attributes)
        {
                switch (pair.second.type)
                {
                case ShaderAttributeListDescription::ValueType::FLOAT:
                        memcpy(&Data[pair.second.offset], &data.GetFloat(pair.first), static_cast<std::size_t>(ShaderAttributeListDescription::ValueType::FLOAT));
                        break;

                case ShaderAttributeListDescription::ValueType::VEC2:
                        memcpy(&Data[pair.second.offset], glm::value_ptr(data.GetVec2(pair.first)), static_cast<std::size_t>(ShaderAttributeListDescription::ValueType::VEC2));
                        break;

                case ShaderAttributeListDescription::ValueType::VEC3:
                        memcpy(&Data[pair.second.offset], glm::value_ptr(data.GetVec3(pair.first)), static_cast<std::size_t>(ShaderAttributeListDescription::ValueType::VEC3));
                        break;

                case ShaderAttributeListDescription::ValueType::VEC4:
                        memcpy(&Data[pair.second.offset], glm::value_ptr(data.GetVec4(pair.first)), static_cast<std::size_t>(ShaderAttributeListDescription::ValueType::VEC4));
                        break;

                case ShaderAttributeListDescription::ValueType::MAT3:
                        memcpy(&Data[pair.second.offset], glm::value_ptr(data.GetMat3(pair.first)), static_cast<std::size_t>(ShaderAttributeListDescription::ValueType::MAT3));
                        break;

                case ShaderAttributeListDescription::ValueType::MAT4:
                        memcpy(&Data[pair.second.offset], glm::value_ptr(data.GetMat4(pair.first)), static_cast<std::size_t>(ShaderAttributeListDescription::ValueType::MAT4));
                        break;
                }
        }

        Renderer::GetContext()->UnmapBuffer(buffer, Diligent::MAP_WRITE);
        free(DataVoid);
}
void ShaderAttributeListDescription::SetDefaultAttribs(ShaderAttributeListData& data)
{
        for (const auto& pair : Attributes)
        {
                const auto& name = pair.first;
                switch (pair.second.type)
                {
                case ShaderAttributeListDescription::ValueType::FLOAT:
                        data.SetFloat(name, 0.f);
                        break;

                case ShaderAttributeListDescription::ValueType::VEC2:
                        data.SetVec2(name, glm::vec2(0.f));
                        break;

                case ShaderAttributeListDescription::ValueType::VEC3:
                        data.SetVec3(name, glm::vec3(0.f));
                        break;

                case ShaderAttributeListDescription::ValueType::VEC4:
                        data.SetVec4(name, glm::vec4(0.f));
                        break;

                case ShaderAttributeListDescription::ValueType::MAT3:
                        data.SetMat3(name, glm::mat3(0.f));
                        break;

                case ShaderAttributeListDescription::ValueType::MAT4:
                        data.SetMat4(name, glm::mat4(0.f));
                        break;
                }
        }
}
}