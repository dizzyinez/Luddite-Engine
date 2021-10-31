#include "Luddite/Graphics/ShaderAttributeList.hpp"
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

// void ShaderBufferDescription::MapTextures(ShaderBufferData& data, Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> srb)
// {
//         for (auto& tex_name : Textures)
//         {
//                 // LD_LOG_TRACE("MAPPING TEXTURE {}", tex_name);
//                 auto tex = data.GetTexture(tex_name).get()->GetTexture();
//                 auto var = srb->GetVariableByName(Diligent::SHADER_TYPE_PIXEL, tex_name.c_str());
//                 var->Set(tex->GetDefaultView(Diligent::TEXTURE_VIEW_SHADER_RESOURCE));
//         }
// }
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
