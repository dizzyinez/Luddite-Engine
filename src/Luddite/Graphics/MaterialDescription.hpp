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
struct LUDDITE_API MaterialDescription
{
        enum class AlphaMode : uint8_t
        {
                OPAQUE = 0, //Opaque material
                SEMITRANSPARENT, //Rendered in material pass with lighting calculations, transparency is created by "killing" pixels
                TRANSLUCENT //Rendered in its own subpass without lighting calculations but with reflections.
        };

        AlphaMode AlphaMode;
        ColorRGB Diffuse;
        ColorRGB Specular;
        ColorRGB Emmission;
        float Metallic = 0.f;
        float Roughness = 1.f;
        float Alpha = 1.f;

        std::string Name{};
};
}