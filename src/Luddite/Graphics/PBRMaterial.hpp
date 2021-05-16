#pragma once
#include "Luddite/pch.hpp"
#include "Luddite/Core.hpp"

#include "Luddite/Graphics/Color.hpp"
#pragma once
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
class LUDDITE_API PBRMaterial
{
public:
        enum class WORKFLOW : int
        {
                METALLIC = 0,
                DIELECTRIC
        }


private:
};
}