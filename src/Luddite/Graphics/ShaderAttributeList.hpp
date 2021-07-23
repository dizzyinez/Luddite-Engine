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
struct LUDDITE_API ShaderAttributeListData
{
        std::string Name{};
        std::unordered_map<std::string, Texture::Handle> texture_map;
        Texture::Handle GetTexture(const std::string& id) {return texture_map[id];}
        void SetTexture(const std::string& id, const Texture::Handle handle) {texture_map[id] = handle;}

        char* Data = nullptr;
        std::size_t Size = 0;
        ~ShaderAttributeListData()
        {
                if (Data)
                        free(Data);
        }

        ShaderAttributeListData& operator=(const ShaderAttributeListData& other)
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
                texture_map = other.texture_map;
                return *this;
        }
};

struct LUDDITE_API ShaderAttributeListDescription
{
        enum class ValueType
        {
                FLOAT,
                INT,
                UINT,
                VEC2,
                IVEC2,
                UVEC2,
                VEC3,
                IVEC3,
                UVEC3,
                VEC4,
                IVEC4,
                UVEC4,
                MAT3,
                MAT4
        };

        struct BufferValue
        {
                ValueType type;
                std::size_t offset;
        };
        /* VVVVVVVVVV WARNING! DISGUSTING BOILER-PLATE CODE AHEAD VVVVVVVVVV */
        std::map<std::string, BufferValue> Attributes;
        std::vector<std::string> Textures;

        inline void AddTexture(const std::string& name) {Textures.emplace_back(name);}
        inline void AddFloat(const std::string& name) {Attributes.insert({name, {ValueType::FLOAT, m_Size}});   m_Size += 4;}
        inline void AddInt(const std::string& name) {Attributes.insert({name, {ValueType::INT, m_Size}});       m_Size += 4;}
        inline void AddUInt(const std::string& name) {Attributes.insert({name, {ValueType::UINT, m_Size}});     m_Size += 4;}
        inline void AddVec2(const std::string& name) {Attributes.insert({name, {ValueType::VEC2, m_Size}});     m_Size += 4 * 2;}
        inline void AddIVec2(const std::string& name) {Attributes.insert({name, {ValueType::IVEC2, m_Size}});   m_Size += 4 * 2;}
        inline void AddUVec2(const std::string& name) {Attributes.insert({name, {ValueType::UVEC2, m_Size}});   m_Size += 4 * 2;}
        inline void AddVec3(const std::string& name) {Attributes.insert({name, {ValueType::VEC3, m_Size}});     m_Size += 4 * 3;}
        inline void AddIVec3(const std::string& name) {Attributes.insert({name, {ValueType::IVEC3, m_Size}});   m_Size += 4 * 3;}
        inline void AddUVec3(const std::string& name) {Attributes.insert({name, {ValueType::UVEC3, m_Size}});   m_Size += 4 * 3;}
        inline void AddVec4(const std::string& name) {Attributes.insert({name, {ValueType::VEC4, m_Size}});     m_Size += 4 * 4;}
        inline void AddIVec4(const std::string& name) {Attributes.insert({name, {ValueType::IVEC4, m_Size}});   m_Size += 4 * 4;}
        inline void AddUVec4(const std::string& name) {Attributes.insert({name, {ValueType::UVEC4, m_Size}});   m_Size += 4 * 4;}
        inline void AddMat3(const std::string& name) {Attributes.insert({name, {ValueType::MAT3, m_Size}});     m_Size += 4 * 3 * 3;}
        inline void AddMat4(const std::string& name) {Attributes.insert({name, {ValueType::MAT4, m_Size}});     m_Size += 4 * 4 * 4;}

        inline void SetFloat(ShaderAttributeListData& data, const std::string& name, float value) {std::size_t offset = Attributes.find(name)->second.offset;      float value_ = value;      memcpy(data.Data + offset, &value_, 4);}
        inline void SetInt(ShaderAttributeListData& data, const std::string& name, int32_t value) {std::size_t offset = Attributes.find(name)->second.offset;      int32_t value_ = value;    memcpy(data.Data + offset, &value_, 4);}
        inline void SetUInt(ShaderAttributeListData& data, const std::string& name, uint32_t value) {std::size_t offset = Attributes.find(name)->second.offset;    uint32_t value_ = value;   memcpy(data.Data + offset, &value_, 4);}
        inline void SetVec2(ShaderAttributeListData& data, const std::string& name, glm::vec2 value) {std::size_t offset = Attributes.find(name)->second.offset;   glm::vec2 value_ = value;  memcpy(data.Data + offset, glm::value_ptr(value_), 4 * 2);}
        inline void SetIVec2(ShaderAttributeListData& data, const std::string& name, glm::ivec2 value) {std::size_t offset = Attributes.find(name)->second.offset; glm::ivec2 value_ = value; memcpy(data.Data + offset, glm::value_ptr(value_), 4 * 2);}
        inline void SetUVec2(ShaderAttributeListData& data, const std::string& name, glm::uvec2 value) {std::size_t offset = Attributes.find(name)->second.offset; glm::uvec2 value_ = value; memcpy(data.Data + offset, glm::value_ptr(value_), 4 * 2);}
        inline void SetVec3(ShaderAttributeListData& data, const std::string& name, glm::vec3 value) {std::size_t offset = Attributes.find(name)->second.offset;   glm::vec3 value_ = value;  memcpy(data.Data + offset, glm::value_ptr(value_), 4 * 3);}
        inline void SetIVec3(ShaderAttributeListData& data, const std::string& name, glm::ivec3 value) {std::size_t offset = Attributes.find(name)->second.offset; glm::ivec3 value_ = value; memcpy(data.Data + offset, glm::value_ptr(value_), 4 * 3);}
        inline void SetUVec3(ShaderAttributeListData& data, const std::string& name, glm::uvec3 value) {std::size_t offset = Attributes.find(name)->second.offset; glm::uvec3 value_ = value; memcpy(data.Data + offset, glm::value_ptr(value_), 4 * 3);}
        inline void SetVec4(ShaderAttributeListData& data, const std::string& name, glm::vec4 value) {std::size_t offset = Attributes.find(name)->second.offset;   glm::vec4 value_ = value;  memcpy(data.Data + offset, glm::value_ptr(value_), 4 * 4);}
        inline void SetIVec4(ShaderAttributeListData& data, const std::string& name, glm::ivec4 value) {std::size_t offset = Attributes.find(name)->second.offset; glm::ivec4 value_ = value; memcpy(data.Data + offset, glm::value_ptr(value_), 4 * 4);}
        inline void SetUVec4(ShaderAttributeListData& data, const std::string& name, glm::uvec4 value) {std::size_t offset = Attributes.find(name)->second.offset; glm::uvec4 value_ = value; memcpy(data.Data + offset, glm::value_ptr(value_), 4 * 4);}
        inline void SetMat3(ShaderAttributeListData& data, const std::string& name, glm::mat3 value) {std::size_t offset = Attributes.find(name)->second.offset;   glm::mat3 value_ = value;  memcpy(data.Data + offset, glm::value_ptr(value_), 4 * 3 * 3);}
        inline void SetMat4(ShaderAttributeListData& data, const std::string& name, glm::mat4 value) {std::size_t offset = Attributes.find(name)->second.offset;   glm::mat4 value_ = value;  memcpy(data.Data + offset, glm::value_ptr(value_), 4 * 4 * 4);}

        inline float      GetFloat(ShaderAttributeListData& data, const std::string& name) {std::size_t offset = Attributes.find(name)->second.offset; float ret;      memcpy(&ret, data.Data + offset, 4); return ret;}
        inline int32_t    GetInt(ShaderAttributeListData& data, const std::string& name) {std::size_t offset = Attributes.find(name)->second.offset; int32_t ret;      memcpy(&ret, data.Data + offset, 4); return ret;}
        inline uint32_t   GetUInt(ShaderAttributeListData& data, const std::string& name) {std::size_t offset = Attributes.find(name)->second.offset; uint32_t ret;    memcpy(&ret, data.Data + offset, 4); return ret;}
        inline glm::vec2  GetVec2(ShaderAttributeListData& data, const std::string& name) {std::size_t offset = Attributes.find(name)->second.offset; glm::vec2 ret;   memcpy(glm::value_ptr(ret), data.Data + offset, 4 * 2); return ret;}
        inline glm::ivec2 GetIVec2(ShaderAttributeListData& data, const std::string& name) {std::size_t offset = Attributes.find(name)->second.offset; glm::ivec2 ret; memcpy(glm::value_ptr(ret), data.Data + offset, 4 * 2); return ret;}
        inline glm::uvec2 GetUVec2(ShaderAttributeListData& data, const std::string& name) {std::size_t offset = Attributes.find(name)->second.offset; glm::uvec2 ret; memcpy(glm::value_ptr(ret), data.Data + offset, 4 * 2); return ret;}
        inline glm::vec3  GetVec3(ShaderAttributeListData& data, const std::string& name) {std::size_t offset = Attributes.find(name)->second.offset; glm::vec3 ret;   memcpy(glm::value_ptr(ret), data.Data + offset, 4 * 3); return ret;}
        inline glm::ivec3 GetIVec3(ShaderAttributeListData& data, const std::string& name) {std::size_t offset = Attributes.find(name)->second.offset; glm::ivec3 ret; memcpy(glm::value_ptr(ret), data.Data + offset, 4 * 3); return ret;}
        inline glm::uvec3 GetUVec3(ShaderAttributeListData& data, const std::string& name) {std::size_t offset = Attributes.find(name)->second.offset; glm::uvec3 ret; memcpy(glm::value_ptr(ret), data.Data + offset, 4 * 3); return ret;}
        inline glm::vec4  GetVec4(ShaderAttributeListData& data, const std::string& name) {std::size_t offset = Attributes.find(name)->second.offset; glm::vec4 ret;   memcpy(glm::value_ptr(ret), data.Data + offset, 4 * 4); return ret;}
        inline glm::ivec4 GetIVec4(ShaderAttributeListData& data, const std::string& name) {std::size_t offset = Attributes.find(name)->second.offset; glm::ivec4 ret; memcpy(glm::value_ptr(ret), data.Data + offset, 4 * 4); return ret;}
        inline glm::uvec4 GetUVec4(ShaderAttributeListData& data, const std::string& name) {std::size_t offset = Attributes.find(name)->second.offset; glm::uvec4 ret; memcpy(glm::value_ptr(ret), data.Data + offset, 4 * 4); return ret;}
        inline glm::mat3  GetMat3(ShaderAttributeListData& data, const std::string& name) {std::size_t offset = Attributes.find(name)->second.offset; glm::mat3 ret;   memcpy(glm::value_ptr(ret), data.Data + offset, 4 * 3 * 3); return ret;}
        inline glm::mat4  GetMat4(ShaderAttributeListData& data, const std::string& name) {std::size_t offset = Attributes.find(name)->second.offset; glm::mat4 ret;   memcpy(glm::value_ptr(ret), data.Data + offset, 4 * 4 * 4); return ret;}

        std::size_t GetSize() const
        {
                return m_Size;
        }
        void MapBuffer(ShaderAttributeListData& data, Diligent::RefCntAutoPtr<Diligent::IBuffer> buffer);
        void MapTextures(ShaderAttributeListData& data, Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> srb);
        void SetDefaultAttribs(ShaderAttributeListData& data);

        ShaderAttributeListData CreateData();
        private:
        std::size_t m_Size = 0;
};
}