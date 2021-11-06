#include "Luddite/Graphics/ShaderAttributeList.hpp"
#include "GraphicsTypes.h"
#include "Luddite/Core/Asset.hpp"
#include "Luddite/Graphics/Renderer.hpp"
namespace Luddite
{
void ShaderBufferDescription::MapBuffer(ShaderBufferData& data, Diligent::RefCntAutoPtr<Diligent::IBuffer> buffer) const
{
        if (GetSize() == 0)
        {
                LD_LOG_WARN("Tried to map buffer: \"{}\", but size was 0!", data.Name);
                return;
        }
        LD_VERIFY(GetSize() == data.Size, "Tried to map buffer \"{}\", but its size ({}) didn't match the size of the description ({})", data.Name, data.Size, GetSize());
        char* Data;
        Renderer::GetContext()->MapBuffer(buffer, Diligent::MAP_WRITE, Diligent::MAP_FLAG_DISCARD, (Diligent::PVoid&)Data);

        memcpy(Data, data.Data, data.Size);

        Renderer::GetContext()->UnmapBuffer(buffer, Diligent::MAP_WRITE);
}

void ShaderBufferDescription::SetTextureVertexShader(ShaderBufferData& data, const std::string& name, Handle<Texture> texture)
{
        auto it = std::find(TexturesVertexShader.begin(), TexturesVertexShader.end(), name);
        if (it == TexturesVertexShader.end())
                LD_LOG_WARN("Tried to set non-existant texture {} for shader buffer dat {}", name, data.Name);
        auto index = std::distance(TexturesVertexShader.begin(), it);
        data.TexturesVertexShader[index] = texture;
}

void ShaderBufferDescription::SetTexturePixelShader(ShaderBufferData& data, const std::string& name, Handle<Texture> texture)
{
        auto it = std::find(TexturesPixelShader.begin(), TexturesPixelShader.end(), name);
        if (it == TexturesPixelShader.end())
                LD_LOG_WARN("Tried to set non-existant texture {} for shader buffer dat {}", name, data.Name);
        auto index = std::distance(TexturesPixelShader.begin(), it);
        data.TexturesPixelShader[index] = texture;
}


Handle<Texture> ShaderBufferDescription::GetTextureVertexShader(ShaderBufferData& data, const std::string& name)
{
        auto it = std::find(TexturesVertexShader.begin(), TexturesVertexShader.end(), name);
        if (it == TexturesVertexShader.end())
                return Handle<Texture>(nullptr);
        auto index = std::distance(TexturesVertexShader.begin(), it);
        return data.TexturesVertexShader[index];
}

Handle<Texture> ShaderBufferDescription::GetTexturePixelShader(ShaderBufferData& data, const std::string& name)
{
        auto it = std::find(TexturesPixelShader.begin(), TexturesPixelShader.end(), name);
        if (it == TexturesPixelShader.end())
                return Handle<Texture>(nullptr);
        auto index = std::distance(TexturesPixelShader.begin(), it);
        return data.TexturesPixelShader[index];
}

void ShaderBufferDescription::MapTextures(ShaderBufferData& data, Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> srb)
{
        for (auto& tex_name : TexturesVertexShader)
        {
                // LD_LOG_TRACE("MAPPING TEXTURE {}", tex_name);
                auto tex = GetTextureVertexShader(data, tex_name);
                if (!tex->Valid())
                        continue;
                auto var = srb->GetVariableByName(Diligent::SHADER_TYPE_PIXEL, tex_name.c_str());
                var->Set(tex->m_pTexture->GetDefaultView(Diligent::TEXTURE_VIEW_SHADER_RESOURCE));
        }
        for (auto& tex_name : TexturesPixelShader)
        {
                // LD_LOG_TRACE("MAPPING TEXTURE {}", tex_name);
                auto tex = GetTexturePixelShader(data, tex_name);
                if (!tex->Valid())
                        continue;
                auto var = srb->GetVariableByName(Diligent::SHADER_TYPE_PIXEL, tex_name.c_str());
                var->Set(tex->m_pTexture->GetDefaultView(Diligent::TEXTURE_VIEW_SHADER_RESOURCE));
        }
}
const char* ShaderBufferDescription::ValueTypeNames[] =
{
                #define VALUE_TYPE_DECLARATION(Name, Type, Default) #Name,
        VALUE_TYPES_DECLARE
                #undef VALUE_TYPE_DECLARATION
};

ShaderBufferData ShaderBufferDescription::CreateData(const std::string& name) const
{
        ShaderBufferData new_data;
        new_data.Name = name;
        new_data.Size = m_Size;
        new_data.Data = (char*)malloc(m_Size);
        // LD_LOG_TRACE("SIZE: {}", m_Size);
        SetDefaultAttribs(new_data);
        new_data.TexturesVertexShader.resize(TexturesVertexShader.size(), Handle<Texture>(nullptr));
        new_data.TexturesPixelShader.resize(TexturesPixelShader.size(), Handle<Texture>(nullptr));
        return new_data;
}

ShaderBufferDescription::ValueType ShaderBufferDescription::GetTypenameFromString(const std::string& type) const
{
        for (uint32_t i = 0; i < static_cast<uint32_t>(ValueType::SIZE); i++)
        {
                if (type == ValueTypeNames[i])
                {
                        return static_cast<ValueType>(i);
                }
        }
        return ShaderBufferDescription::ValueType::NONE;
}

void ShaderBufferDescription::SetDefaultAttribs(ShaderBufferData& data) const
{
        for (const auto& pair : Attributes)
        {
                const auto& name = pair.first;
                switch (pair.second.type)
                {
                #define VALUE_TYPE_DECLARATION(Name, Type, Default) \
case ShaderBufferDescription::ValueType::Name: \
        Set ## Name(data, name, Default); \
        break;
                        VALUE_TYPES_DECLARE
                #undef VALUE_TYPE_DECLARATION
                }
        }
}
}
