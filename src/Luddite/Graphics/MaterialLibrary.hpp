#pragma once
#include "Luddite/pch.hpp"
#include "Luddite/Core.hpp"
#include "Luddite/BasicAllocator.hpp"
#include "Luddite/Graphics/Material.hpp"
#include "Luddite/Graphics/ShaderAttributeList.hpp"


namespace Luddite
{
class LUDDITE_API MaterialLibrary : public BasicAllocator<Material, std::string>
{
public:
        MaterialLibrary(ShaderAttributeListDescription MaterialShaderAttributes);
        MaterialHandle Allocate(const std::string& id) override;
        ShaderAttributeListDescription m_MaterialShaderAttributes;
        ShaderAttributeListData DefaultMaterialData;
private:
};
}