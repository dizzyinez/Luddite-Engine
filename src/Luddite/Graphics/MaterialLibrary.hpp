#pragma once
#include "Luddite/pch.hpp"
#include "Luddite/Core.hpp"
#include "Luddite/BasicAllocator.hpp"

#include "Luddite/Graphics/MaterialDescription.hpp"


namespace Luddite
{
class LUDDITE_API MaterialData
{
        MaterialDescription m_MaterialDescription;
};

class LUDDITE_API Material
{
public:
private:
        std::shared_ptr<MaterialData> m_pMaterialData;
};






class LUDDITE_API MaterialLibrary
{
public:

private:
        class MaterialAllocator : public BasicAllocator<MaterialData, uint32_t>
        {
                std::shared_ptr<MaterialData> Allocate(const uint32_t& id) override {}
        };
        MaterialAllocator m_MaterialAllocator;
};
}