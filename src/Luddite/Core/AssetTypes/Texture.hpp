#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/Core/Core.hpp"
#include "Luddite/Graphics/DiligentInclude.hpp"
#include "Luddite/Core/AssetLibrary.hpp"

namespace Luddite
{
struct LUDDITE_API Texture
{
        Diligent::RefCntAutoPtr<Diligent::ITexture> m_pTexture;// {nullptr};
        void TransitionToShaderResource();
        bool Valid() const {return m_pTexture;}
        Diligent::RefCntAutoPtr<Diligent::ITexture> GetTexture() const {return m_pTexture;}
};

class LUDDITE_API TextureLibrary : public AssetLibrary<Texture, TextureLibrary>
{
        public:
        // virtual void Initialize() override;
        virtual Texture* LoadFromFile(const std::filesystem::path& path) override;
        // virtual void AfterLoadProcessing(BasicModel* pModel) override;
        private:
};
}
