#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/Core/Core.hpp"
#include "Luddite/Graphics/DiligentInclude.hpp"
#include "Luddite/Core/AssetLibrary.hpp"
#include "Luddite/Graphics/ShaderAttributeList.hpp"

namespace Luddite
{
struct LUDDITE_API Shader : public Asset<Shader>
{
        std::string name;
        Diligent::RefCntAutoPtr<Diligent::IShader> m_pVertexShader;
        Diligent::RefCntAutoPtr<Diligent::IShader> m_pPixelShader;
        Diligent::RefCntAutoPtr<Diligent::IPipelineState> m_pPSO;
        ShaderAttributeListDescription m_VertexShaderProperties;
        ShaderAttributeListDescription m_PixelShaderProperties;
        // Diligent::RefCntAutoPtr<Diligent::ITexture> m_pTexture;// {nullptr};
        // void TransitionToShaderResource();
        // bool Valid() const {return m_pTexture;}
        // Diligent::RefCntAutoPtr<Diligent::ITexture> GetTexture() const {return m_pTexture;}
};

class LUDDITE_API ShaderLibrary : public AssetLibrary<Shader, ShaderLibrary>
{
        public:
        virtual void Initialize() override;
        virtual Shader* LoadFromFile(const std::filesystem::path& path) override;
        // virtual void AfterLoadProcessing(BasicModel* pModel) override;
        private:
};
}