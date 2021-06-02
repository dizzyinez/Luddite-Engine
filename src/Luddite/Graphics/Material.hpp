#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/Core/Core.hpp"
#include "Luddite/Core/BasicAllocator.hpp"

#include "Luddite/Graphics/ShaderAttributeList.hpp"
#include "Luddite/Graphics/DiligentInclude.hpp"


namespace Luddite
{
struct Material
{
        ShaderAttributeListData m_data;
        Diligent::RefCntAutoPtr<Diligent::IBuffer> m_pMaterialConstantsBuffer;
        Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> m_pMaterialShaderResourceBinding;
};
typedef std::shared_ptr<Material> MaterialHandle;
}