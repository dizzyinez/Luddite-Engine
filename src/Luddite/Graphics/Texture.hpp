#pragma once
#include "Luddite/pch.hpp"
#include "Luddite/Core.hpp"
#include "Luddite/Graphics/DiligentInclude.hpp"
// #include "Luddite/Core.hpp"

namespace Luddite
{
// using Shader = Diligent::RefCntAutoPtr<Diligent::IShader>;
// using Texture = Diligent::RefCntAutoPtr<ITexture>;
class Texture
{
public:
        // Texture

        Diligent::RefCntAutoPtr<Diligent::ITexture> GetTexture() const {return m_pTexture;}
        void LoadTexture(Diligent::RefCntAutoPtr<Diligent::IRenderDevice> pDevice, const std::string& Path);
private:
        Diligent::RefCntAutoPtr<Diligent::ITexture> m_pTexture;// {nullptr};
};
}