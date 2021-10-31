#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/Core/Core.hpp"
#include "Luddite/Graphics/DiligentInclude.hpp"
#include "Luddite/Core/AssetLibrary.hpp"
#include "Luddite/Graphics/ShaderAttributeList.hpp"
#include "Luddite/Core/AssetTypes/Shader.hpp"

namespace Luddite
{
struct LUDDITE_API Material
{
        ~Material();
        std::string m_Name;
        Handle<Shader> m_pShader;
        // Diligent::RefCntAutoPtr<Diligent::IShader> m_pVertexShader;
        // Diligent::RefCntAutoPtr<Diligent::IShader> m_pPixelShader;
        // Diligent::RefCntAutoPtr<Diligent::IPipelineState> m_pPSO;
        ShaderBufferData m_Properties;

        // Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> m_pMaterialShaderResourceBinding;

        // Diligent::RefCntAutoPtr<Diligent::ITexture> m_pTexture;// {nullptr};
        // void TransitionToShaderResource();
        // bool Valid() const {return m_pTexture;}
        // Diligent::RefCntAutoPtr<Diligent::ITexture> GetTexture() const {return m_pTexture;}
};

class LUDDITE_API MaterialLibrary : public AssetLibrary<Material, MaterialLibrary>
{
        public:
        virtual void Initialize() override;
        virtual Material* LoadFromFile(const std::filesystem::path& path) override;
        // virtual void AfterLoadProcessing(BasicModel* pModel) override;
        private:
};
}
