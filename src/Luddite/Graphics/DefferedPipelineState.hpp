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
class DefferedPipelineState
{
public:
        void Initialize(
                Diligent::RefCntAutoPtr<Diligent::IRenderPass> pRenderPass,
                const std::string& VSFilePath,
                const std::string& PSFilePath,
                const std::string& Name,
                ShaderAttributeListDescription ConstantShaderAttributes,
                ShaderAttributeListDescription MaterialShaderAttributes
                );
        void PrepareDraw();
        void DrawBasicMesh(const BasicMeshHandle Mesh);
        void SetMaterial(MaterialHandle Material);
        void SetViewProjMatrix(const glm::mat4& ViewProj);
        // void SetModelAttribs();
        MaterialHandle GetMaterial(const std::string& Name);
private:
        Diligent::RefCntAutoPtr<Diligent::IPipelineState> m_pPSO;

        ShaderAttributeListDescription m_ConstantShaderAttributes;
        ShaderAttributeListData m_ConstantShaderData;
        Diligent::RefCntAutoPtr<Diligent::IBuffer> m_pShaderConstantsBuffer;

        std::unique_ptr<MaterialLibrary> m_pMaterialLibrary;
        MaterialHandle m_pCurrentMaterial;
};
}