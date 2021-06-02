#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/Core/Core.hpp"
#include "Luddite/Core/BasicAllocator.hpp"
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