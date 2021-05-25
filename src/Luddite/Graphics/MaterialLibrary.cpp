#include "Luddite/Graphics/MaterialLibrary.hpp"

namespace Luddite
{
MaterialLibrary::MaterialLibrary(ShaderAttributeListDescription MaterialShaderAttributes)
{
        m_MaterialShaderAttributes = MaterialShaderAttributes;
        for (const std::string& name : m_MaterialShaderAttributes.floatlist)
                DefaultMaterialData.SetFloat(name, 0.f);
        for (const std::string& name : m_MaterialShaderAttributes.vec2list)
                DefaultMaterialData.SetVec2(name, glm::vec2(0.f));
        for (const std::string& name : m_MaterialShaderAttributes.vec3list)
                DefaultMaterialData.SetVec3(name, glm::vec3(0.f));
        for (const std::string& name : m_MaterialShaderAttributes.vec4list)
                DefaultMaterialData.SetVec4(name, glm::vec4(0.f));
        for (const std::string& name : m_MaterialShaderAttributes.mat3list)
                DefaultMaterialData.SetMat3(name, glm::mat3(0.f));
        for (const std::string& name : m_MaterialShaderAttributes.mat4list)
                DefaultMaterialData.SetMat4(name, glm::mat4(0.f));
}
MaterialHandle MaterialLibrary::Allocate(const std::string& id)
{
        MaterialHandle material = std::make_shared<Material>();
        material->m_data = DefaultMaterialData;
        return material;
}
}