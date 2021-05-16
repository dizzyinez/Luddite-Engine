#include "Luddite/Graphics/Texture.hpp"
#include "Graphics/GraphicsTools/interface/GraphicsUtilities.h"
#include "Graphics/GraphicsTools/interface/MapHelper.hpp"
#include "Graphics/GraphicsTools/interface/TextureUploader.hpp"
#include "Graphics/GraphicsTools/interface/DynamicTextureAtlas.h"
#include "TextureLoader/interface/TextureUtilities.h"

namespace Luddite
{
void Texture::LoadTexture(Diligent::RefCntAutoPtr<Diligent::IRenderDevice> pDevice, const std::string& Path)
{
        Diligent::TextureLoadInfo load_info;
        load_info.IsSRGB = true;
        Diligent::CreateTextureFromFile(Path.c_str(), load_info, pDevice, &m_pTexture);
}
}