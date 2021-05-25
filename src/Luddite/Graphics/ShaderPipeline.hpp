#pragma once
#include "Luddite/pch.hpp"
#include "Luddite/Core.hpp"

#include "Luddite/Graphics/Texture.hpp"
#include "Luddite/Graphics/ShaderAttributeList.hpp"
#include "Luddite/Graphics/MaterialLibrary.hpp"
#include "Luddite/Graphics/Model.hpp"

#include "Luddite/Graphics/DiligentInclude.hpp"

namespace Luddite
{
class ShaderPipeline
{
public:
        void Initialize(
                Diligent::RefCntAutoPtr<Diligent::IRenderDevice> pDevice,
                Diligent::RefCntAutoPtr<Diligent::IRenderPass> pRenderPass,
                Diligent::RefCntAutoPtr<Diligent::IShaderSourceInputStreamFactory> pShaderSourceFactory,
                const std::string& VSFilePath,
                const std::string& PSFilePath,
                const std::string& Name,
                // Diligent::LayoutElement* ShaderLayoutElements,
                ShaderAttributeListDescription ConstantShaderAttributes,
                ShaderAttributeListDescription MaterialShaderAttributes
                );
        void PrepareDraw();
        void DrawBasicMesh(const BasicMesh& Mesh);
        void SetMaterial(MaterialHandle Material);
        // void SetModelAttribs();
        MaterialHandle GetMaterial(const std::string& Name);
private:
        Diligent::RefCntAutoPtr<Diligent::IRenderDevice> m_pDevice;
        Diligent::RefCntAutoPtr<Diligent::IBuffer> m_pShaderConstantsBuffer;
        Diligent::RefCntAutoPtr<Diligent::IBuffer> m_pMaterialConstantsBuffer;


        ShaderAttributeListDescription m_ConstantShaderAttributes;
        ShaderAttributeListDescription m_MaterialShaderAttributes;
        std::unique_ptr<MaterialLibrary> m_pMaterialLibrary;
};
}