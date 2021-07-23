#include "Luddite/Graphics/MaterialLibrary.hpp"

namespace Luddite
{
MaterialLibrary::MaterialLibrary(ShaderAttributeListDescription MaterialShaderAttributes)
{
        m_MaterialShaderAttributes = MaterialShaderAttributes;
        DefaultMaterialData = m_MaterialShaderAttributes.CreateData();
}
MaterialHandle MaterialLibrary::Allocate(const std::string& id)
{
        MaterialHandle material = std::make_shared<Material>();
        material->m_data = DefaultMaterialData;
        return material;
}
}