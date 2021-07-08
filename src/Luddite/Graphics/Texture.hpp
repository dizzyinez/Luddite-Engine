#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/Core/Core.hpp"
#include "Luddite/Graphics/DiligentInclude.hpp"
// #include "Luddite/Core/Core.hpp"

// namespace Luddite
// {
// // using Shader = Diligent::RefCntAutoPtr<Diligent::IShader>;
// // using Texture = Diligent::RefCntAutoPtr<ITexture>;
// class Texture
// {
//         public:
//         Texture() = default;
//         Texture(const std::string& Path) {LoadTexture(Path);}
//         Texture(Diligent::RefCntAutoPtr<Diligent::ITexture> texture_ref) : m_pTexture(texture_ref) {}

//         void TransitionToShaderResource();

//         Diligent::RefCntAutoPtr<Diligent::ITexture> GetTexture() const {return m_pTexture;}
//         void LoadTexture(const std::string& Path);

//         private:
//         Diligent::RefCntAutoPtr<Diligent::ITexture> m_pTexture;// {nullptr};
// };
// }