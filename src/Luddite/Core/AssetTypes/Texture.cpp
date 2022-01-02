#include "Luddite/Core/AssetTypes/Texture.hpp"
#include "Luddite/Graphics/Renderer.hpp"

#include "Graphics/GraphicsTools/interface/TextureUploader.hpp"
#include "TextureLoader/interface/TextureUtilities.h"
namespace Luddite
{
void Texture::TransitionToShaderResource()
{
        if (m_pTexture)
        {
                Diligent::StateTransitionDesc Barriers[] =
                {
                        {m_pTexture, Diligent::RESOURCE_STATE_UNKNOWN, Diligent::RESOURCE_STATE_SHADER_RESOURCE, true}
                };
                Renderer::GetContext()->TransitionResourceStates(_countof(Barriers), Barriers);
        }
}

void TextureLibrary::Initialize()
{
        m_AssetBaseDir = "./Assets/Textures/";
        m_Extensions.emplace(L".png");
        m_Extensions.emplace(L".jpg");
        m_Extensions.emplace(L".jpeg");
        m_Extensions.emplace(L".dds");
        m_Extensions.emplace(L".tiff");
}

Texture* TextureLibrary::LoadFromFile(const std::filesystem::path& path)
{
        Texture* tex = new Texture();
        Diligent::TextureLoadInfo load_info;
        load_info.IsSRGB = true;
        Diligent::CreateTextureFromFile(path.string().c_str(), load_info, Renderer::GetDevice(), &tex->m_pTexture);
        return tex;
}
}
