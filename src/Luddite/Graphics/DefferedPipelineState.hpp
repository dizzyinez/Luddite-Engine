#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/Core/Core.hpp"

#include "Luddite/Graphics/Texture.hpp"
#include "Luddite/Graphics/ShaderAttributeList.hpp"
#include "Luddite/Graphics/MaterialLibrary.hpp"
#include "Luddite/Core/AssetTypes/Model.hpp"

#include "Luddite/Graphics/DiligentInclude.hpp"

namespace Luddite
{
class DefferedPipelineState
{
        public:
        void Initialize(
                Diligent::RefCntAutoPtr<Diligent::IRenderPass> pRenderPass,
                const std::string& VSFilePath,
                const std::string& PSFilePath,
                const std::string& Name,
                ShaderAttributeListDescription ConstantShaderAttributes,
                ShaderAttributeListDescription MaterialShaderAttributes,
                ShaderAttributeListDescription ModelShaderAttributes
                );
        void PrepareDraw();
        void UploadModelData();
        void SetMaterial(MaterialHandle Material);
        inline ShaderAttributeListDescription& GetConstantDataDesc() {return m_ConstantShaderAttributes;}
        inline ShaderAttributeListData& GetConstantData() {return m_ConstantShaderData;}
        inline ShaderAttributeListDescription& GetModelDataDesc() {return m_ModelShaderAttributes;}
        inline ShaderAttributeListData& GetModelData() {return m_ModelShaderData;}
        // void SetModelAttribs();
        MaterialHandle GetMaterial(const std::string& Name);
        private:
        Diligent::RefCntAutoPtr<Diligent::IPipelineState> m_pPSO;

        ShaderAttributeListDescription m_ConstantShaderAttributes;
        ShaderAttributeListData m_ConstantShaderData;
        Diligent::RefCntAutoPtr<Diligent::IBuffer> m_pShaderConstantsBuffer;

        ShaderAttributeListDescription m_ModelShaderAttributes;
        ShaderAttributeListData m_ModelShaderData;
        Diligent::RefCntAutoPtr<Diligent::IBuffer> m_pShaderModelBuffer;

        std::unique_ptr<MaterialLibrary> m_pMaterialLibrary;
        MaterialHandle m_pCurrentMaterial;
};
}