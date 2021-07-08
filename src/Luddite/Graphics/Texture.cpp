#include "Luddite/Graphics/Texture.hpp"
#include "Luddite/Graphics/Renderer.hpp"
#include "Graphics/GraphicsTools/interface/GraphicsUtilities.h"
#include "Graphics/GraphicsTools/interface/MapHelper.hpp"
#include "Graphics/GraphicsTools/interface/TextureUploader.hpp"
#include "Graphics/GraphicsTools/interface/DynamicTextureAtlas.h"
#include "TextureLoader/interface/TextureUtilities.h"

// namespace Luddite
// {
// void Texture::LoadTexture(const std::string& Path)
// {
//         Diligent::TextureLoadInfo load_info;
//         load_info.IsSRGB = true;
//         Diligent::CreateTextureFromFile(Path.c_str(), load_info, Renderer::GetDevice(), &m_pTexture);
// }
// void Texture::TransitionToShaderResource()
// {
//         Diligent::StateTransitionDesc Barriers[] =
//         {
//                 {m_pTexture, Diligent::RESOURCE_STATE_UNKNOWN, Diligent::RESOURCE_STATE_SHADER_RESOURCE, true}
//         };
//         Renderer::GetContext()->TransitionResourceStates(_countof(Barriers), Barriers);
// }
// }