#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/Core/Core.hpp"
#include "Luddite/Graphics/DiligentInclude.hpp"
#include "Luddite/Core/AssetLibrary.hpp"
#include "Luddite/Graphics/ShaderAttributeList.hpp"
namespace Luddite
{
struct LUDDITE_API Shader
{
        std::string m_Name;
        Diligent::RefCntAutoPtr<Diligent::IShader> m_pVertexShader;
        Diligent::RefCntAutoPtr<Diligent::IShader> m_pPixelShader;
        Diligent::RefCntAutoPtr<Diligent::IPipelineState> m_pPSO;
        ShaderBufferDescription m_PropertiesBufferDescription;
        Diligent::RefCntAutoPtr<Diligent::IBuffer> m_PropertiesBuffer;
        bool vertex_shader_uses_properties = false;
        bool pixel_shader_uses_properties = false;
        std::vector<std::string> m_MutableBuffersVertex;
        std::vector<std::string> m_MutableBuffersPixel;
        std::vector<std::string> m_ProvidedTexturesVertex;
        std::vector<std::string> m_ProvidedTexturesPixel;
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
        std::unordered_map<std::string, Diligent::RefCntAutoPtr<Diligent::IBuffer> > static_buffers;
        std::unordered_set<std::string> mutable_buffers;
        std::unordered_set<std::string> provided_textures;
};
}
