#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/Core/Core.hpp"
#include "Luddite/Core/AssetTypes/Texture.hpp"
#include "Luddite/Graphics/Color.hpp"
#include "Luddite/Graphics/DiligentInclude.hpp"
// #include "Luddite/Core/pch.hpp"
// #include "Luddite/Platform/DiligentPlatform.hpp"
// #include "Graphics/GraphicsEngine/interface/RenderDevice.h"
// #include "Graphics/GraphicsEngine/interface/DeviceContext.h"
// #include "Graphics/GraphicsEngine/interface/SwapChain.h"

// #include "Common/interface/RefCntAutoPtr.hpp"
// // #include "Luddite/Core/Core.hpp"

namespace Luddite
{
struct LUDDITE_API ShaderBufferData
{
        std::string Name{};
        // std::unordered_map<std::string, Texture::Handle> texture_map;
        // Texture::Handle GetTexture(const std::string& id) {return texture_map[id];}
        // void SetTexture(const std::string& id, const Texture::Handle handle) {texture_map[id] = handle;}

        char* Data = nullptr;
        std::size_t Size = 0;
        ~ShaderBufferData()
        {
                if (Data)
                        free(Data);
        }

        ShaderBufferData& operator=(const ShaderBufferData& other)
        {
                if (this == &other)
                        return *this;
                if (Size != other.Size)
                {
                        if (Data)
                                free(Data);
                        Size = other.Size;
                        Data = (char*)malloc(Size);
                }
                memcpy(Data, other.Data, Size);
                Name = other.Name;
                // texture_map = other.texture_map;
                return *this;
        }
};

//enum, name, typename, size
#define VALUE_TYPES_DECLARE \
        VALUE_TYPE_DECLARATION(Float, float, 0.f) \
        VALUE_TYPE_DECLARATION(Int, int32_t, 0) \
        VALUE_TYPE_DECLARATION(UInt, uint32_t, 0) \
        VALUE_TYPE_DECLARATION(Vec2, glm::vec2, glm::vec2{0}) \
        VALUE_TYPE_DECLARATION(IVec2, glm::ivec2, glm::ivec2{0}) \
        VALUE_TYPE_DECLARATION(UVec2, glm::uvec2, glm::uvec2{0}) \
        VALUE_TYPE_DECLARATION(Vec3, glm::vec3, glm::vec3{0}) \
        VALUE_TYPE_DECLARATION(IVec3, glm::ivec3, glm::ivec3{0}) \
        VALUE_TYPE_DECLARATION(UVec3, glm::uvec3, glm::uvec3{0}) \
        VALUE_TYPE_DECLARATION(Vec4, glm::vec4, glm::vec4{0}) \
        VALUE_TYPE_DECLARATION(Color, glm::vec4, glm::vec4{1}) \
        VALUE_TYPE_DECLARATION(IVec4, glm::ivec4, glm::ivec4{0}) \
        VALUE_TYPE_DECLARATION(UVec4, glm::uvec4, glm::uvec4{0}) \
        VALUE_TYPE_DECLARATION(Mat3, glm::mat3, glm::identity<glm::mat3>()) \
        VALUE_TYPE_DECLARATION(Mat4, glm::mat4, glm::identity<glm::mat3>())


struct LUDDITE_API ShaderBufferDescription
{
        static const char* ValueTypeNames[];

        enum class ValueType : uint32_t
        {
        #define VALUE_TYPE_DECLARATION(Name, Type, Default) Name,
                VALUE_TYPES_DECLARE
        #undef VALUE_TYPE_DECLARATION
                SIZE,
                NONE
        };

        static constexpr uint32_t ValueTypeSizes[] =
        {
                #define VALUE_TYPE_DECLARATION(Name, Type, Default) sizeof(Type),
                VALUE_TYPES_DECLARE
                #undef VALUE_TYPE_DECLARATION
                0,
                0
        };


        struct BufferValue
        {
                ValueType type;
                std::size_t offset;
        };
        std::map<std::string, BufferValue> Attributes;
        // std::vector<std::string> Textures;

        // inline void AddTexture(const std::string& name) {Textures.emplace_back(name);}

        #define VALUE_TYPE_DECLARATION(Name, Type, Default) inline void Add ## Name(const std::string& name) \
        { \
                /*Perform byte alignment*/ \
                constexpr int align = 16; \
                if (m_Size / align != ((m_Size + sizeof(Type) - 1) / align) && m_Size % align != 0) \
                {m_Size = ((m_Size / align) + 1) * align;} \
                Attributes.insert({name, {ValueType::Name, m_Size}}); \
                m_Size += sizeof(Type); \
        }
        VALUE_TYPES_DECLARE
        #undef VALUE_TYPE_DECLARATION
        inline void Add(const std::string& name, const ValueType& type)
        {
                //Perform byte alignment
                constexpr int align = 16;
                if (m_Size / align != ((m_Size + ValueTypeSizes[static_cast<uint32_t>(type)] - 1) / align) && m_Size % align != 0)
                        m_Size = ((m_Size / align) + 1) * align;
                Attributes.insert({name, {type, m_Size}});
                m_Size += ValueTypeSizes[static_cast<uint32_t>(type)];
        }
        ValueType GetTypenameFromString(const std::string& type) const;

        #define VALUE_TYPE_DECLARATION(Name, Type, Default) inline void Set ## Name(ShaderBufferData & data, const std::string& name, Type value) const \
        { \
                std::size_t offset = Attributes.find(name)->second.offset; \
                Type value_ = value; \
                memcpy(data.Data + offset, &value_, sizeof(Type)); \
        }
        VALUE_TYPES_DECLARE
        #undef VALUE_TYPE_DECLARATION

        #define VALUE_TYPE_DECLARATION(Name, Type, Default) inline Type Get ## Name(ShaderBufferData & data, const std::string& name) const\
        { \
                std::size_t offset = Attributes.find(name)->second.offset; \
                Type ret; \
                memcpy(&ret, data.Data + offset, sizeof(Type)); \
                return ret; \
        }
        VALUE_TYPES_DECLARE
        #undef VALUE_TYPE_DECLARATION

        std::size_t GetSize() const
        {
                return m_Size;
        }
        void MapBuffer(ShaderBufferData & data, Diligent::RefCntAutoPtr<Diligent::IBuffer> buffer) const;
        // void MapTextures(ShaderAttributeListData& data, Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> srb);
        void SetDefaultAttribs(ShaderBufferData & data) const;

        ShaderBufferData CreateData(const std::string& name) const;
        private:
        std::size_t m_Size = 0;
};
}
