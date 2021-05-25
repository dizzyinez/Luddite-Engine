#pragma once
#include "Luddite/pch.hpp"
#include "Luddite/Core.hpp"

#include "Luddite/Graphics/Color.hpp"
// #include "Luddite/pch.hpp"
// #include "Luddite/Platform/DiligentPlatform.hpp"
// #include "Graphics/GraphicsEngine/interface/RenderDevice.h"
// #include "Graphics/GraphicsEngine/interface/DeviceContext.h"
// #include "Graphics/GraphicsEngine/interface/SwapChain.h"

// #include "Common/interface/RefCntAutoPtr.hpp"
// // #include "Luddite/Core.hpp"

namespace Luddite
{
// using Shader = Diligent::RefCntAutoPtr<Diligent::IShader>;
// using Texture = Diligent::RefCntAutoPtr<ITexture>;
struct LUDDITE_API ShaderAttributeListDescription
{
        // std::vector<std::string> intlist;
        std::vector<std::string> floatlist;
        inline void AddFloat(const std::string& name) {floatlist.push_back(name);}
        std::vector<std::string> vec2list;
        inline void AddVec2(const std::string& name) {vec2list.push_back(name);}
        std::vector<std::string> vec3list;
        inline void AddVec3(const std::string& name) {vec3list.push_back(name);}
        std::vector<std::string> vec4list;
        inline void AddVec4(const std::string& name) {vec4list.push_back(name);}
        std::vector<std::string> mat3list;
        inline void AddMat3(const std::string& name) {mat3list.push_back(name);}
        std::vector<std::string> mat4list;
        inline void AddMat4(const std::string& name) {mat4list.push_back(name);}
        std::size_t GetSize() const
        {
                return
                        sizeof(float) * floatlist.size() +
                        sizeof(glm::vec2) * vec2list.size() +
                        sizeof(glm::vec3) * vec3list.size() +
                        sizeof(glm::vec4) * vec4list.size() +
                        sizeof(glm::mat3) * mat3list.size() +
                        sizeof(glm::mat4) * mat4list.size();
        }
};
struct LUDDITE_API ShaderAttributeListData
{
        std::string Name{};
        // std::unordered_map<std::string, int> intmap;
        std::unordered_map<std::string, float> floatmap;
        float GetFloat(const std::string& id) {return floatmap[id];}
        void SetFloat(const std::string& id, const float& value) {floatmap[id] = value;}


        std::unordered_map<std::string, glm::vec2> vec2map;
        glm::vec2 GetVec2(const std::string& id) {return vec2map[id];}
        void SetVec2(const std::string& id, const glm::vec2& value) {vec2map[id] = value;}

        std::unordered_map<std::string, glm::vec3> vec3map;
        glm::vec3 GetVec3(const std::string& id) {return vec3map[id];}
        void SetVec3(const std::string& id, const glm::vec3& value) {vec3map[id] = value;}

        std::unordered_map<std::string, glm::vec4> vec4map;
        glm::vec4 GetVec4(const std::string& id) {return vec4map[id];}
        void SetVec4(const std::string& id, const glm::vec4& value) {vec4map[id] = value;}

        std::unordered_map<std::string, glm::mat3> mat3map;
        glm::mat3 GetMat3(const std::string& id) {return mat3map[id];}
        void SetMat3(const std::string& id, const glm::mat3& value) {mat3map[id] = value;}

        std::unordered_map<std::string, glm::mat4> mat4map;
        glm::mat4 GetMat4(const std::string& id) {return mat4map[id];}
        void SetMat4(const std::string& id, const glm::mat4& value) {mat4map[id] = value;}
        // enum class AlphaMode : uint8_t
        // {
        //         OPAQUE = 0, //Opaque material
        //         SEMITRANSPARENT, //Rendered in material pass with lighting calculations, transparency is created by "killing" pixels
        //         TRANSLUCENT //Rendered in its own subpass without lighting calculations but with reflections.
        // };
        // AlphaMode AlphaMode;
        // ColorRGB Diffuse;
        // ColorRGB Specular;
        // ColorRGB Emmission;
        // float Metallic = 0.f;
        // float Roughness = 1.f;
        // float Alpha = 1.f;
};
}