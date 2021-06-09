#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/Core/Core.hpp"

#include "Luddite/Graphics/Color.hpp"
#include "Luddite/Graphics/Texture.hpp"
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
        std::unordered_map<std::string, Texture> texturemap;
        Texture& GetTexture(const std::string& id) {return texturemap[id];}
        void SetTexture(const std::string& id, const Texture& value) {texturemap[id] = value;}

        std::unordered_map<std::string, float> floatmap;
        float& GetFloat(const std::string& id) {return floatmap[id];}
        void SetFloat(const std::string& id, const float& value) {floatmap[id] = value;}

        std::unordered_map<std::string, glm::vec2> vec2map;
        glm::vec2& GetVec2(const std::string& id) {return vec2map[id];}
        void SetVec2(const std::string& id, const glm::vec2& value) {vec2map[id] = value;}

        std::unordered_map<std::string, glm::vec3> vec3map;
        glm::vec3& GetVec3(const std::string& id) {return vec3map[id];}
        void SetVec3(const std::string& id, const glm::vec3& value) {vec3map[id] = value;}

        std::unordered_map<std::string, glm::vec4> vec4map;
        glm::vec4& GetVec4(const std::string& id) {return vec4map[id];}
        void SetVec4(const std::string& id, const glm::vec4& value) {vec4map[id] = value;}

        std::unordered_map<std::string, glm::mat3> mat3map;
        glm::mat3& GetMat3(const std::string& id) {return mat3map[id];}
        void SetMat3(const std::string& id, const glm::mat3& value) {mat3map[id] = value;}

        std::unordered_map<std::string, glm::mat4> mat4map;
        glm::mat4& GetMat4(const std::string& id) {return mat4map[id];}
        void SetMat4(const std::string& id, const glm::mat4& value) {mat4map[id] = value;}
};

struct LUDDITE_API ShaderAttributeListDescription
{
        enum class ValueType : std::size_t
        {
                FLOAT = sizeof(float),
                VEC2 = sizeof(glm::vec2),
                VEC3 = sizeof(glm::vec3),
                VEC4 = sizeof(glm::vec4),
                MAT3 = sizeof(glm::mat3),
                MAT4 = sizeof(glm::mat4)
        };

        struct BufferValue
        {
                ValueType type;
                std::size_t offset;
        };

        std::map<std::string, BufferValue> Attributes;
        std::vector<std::string> Textures;
        inline void AddTexture(const std::string& name) {Textures.emplace_back(name);}
        inline void AddFloat(const std::string& name) {Attributes.insert({name, {ValueType::FLOAT, current_offest}}); current_offest += static_cast<std::size_t>(ValueType::FLOAT);}
        inline void AddVec2(const std::string& name) {Attributes.insert({name, {ValueType::VEC2, current_offest}}); current_offest += static_cast<std::size_t>(ValueType::VEC2);}
        inline void AddVec3(const std::string& name) {Attributes.insert({name, {ValueType::VEC3, current_offest}}); current_offest += static_cast<std::size_t>(ValueType::VEC3);}
        inline void AddVec4(const std::string& name) {Attributes.insert({name, {ValueType::VEC4, current_offest}}); current_offest += static_cast<std::size_t>(ValueType::VEC4);}
        inline void AddMat3(const std::string& name) {Attributes.insert({name, {ValueType::MAT3, current_offest}}); current_offest += static_cast<std::size_t>(ValueType::MAT3);}
        inline void AddMat4(const std::string& name) {Attributes.insert({name, {ValueType::MAT4, current_offest}}); current_offest += static_cast<std::size_t>(ValueType::MAT4);}
        std::size_t GetSize() const
        {
                return current_offest;
        }
        void MapBuffer(ShaderAttributeListData& data, Diligent::RefCntAutoPtr<Diligent::IBuffer> buffer);
        void MapTextures(ShaderAttributeListData& data, Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> srb);
        void SetDefaultAttribs(ShaderAttributeListData& data);
        private:
        std::size_t current_offest = 0;
};
}