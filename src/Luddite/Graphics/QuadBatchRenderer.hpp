#pragma once
#include "Luddite/Core.hpp"
#include "Luddite/pch.hpp"

#include "Luddite/Graphics/Texture.hpp"
#include "Luddite/Graphics/DiligentInclude.hpp"

namespace Luddite
{
class LUDDITE_API QuadBatchRenderer
{
public:
        void Initialize(Diligent::RefCntAutoPtr<Diligent::IRenderDevice> pDevice,
                        Diligent::RefCntAutoPtr<Diligent::IDeviceContext> pImmediateContext,
                        Diligent::TEXTURE_FORMAT RTVFormat,
                        Diligent::TEXTURE_FORMAT DSVFormat,
                        Diligent::RefCntAutoPtr<Diligent::IShaderSourceInputStreamFactory> pShaderSourceFactory,
                        const std::string& VSFilePath,
                        const std::string& PSFilePath);
        void AddQuad(const Texture& texture, const glm::mat4& transform);
        inline void SetViewProjMatrix(const glm::mat4& matrix) {m_ViewProjMatrix = matrix;}
        void StartBatch();
        void UploadBatch();
        void DrawBatch();
private:
        void CreatePipelineState(
                Diligent::TEXTURE_FORMAT RTVFormat,
                Diligent::TEXTURE_FORMAT DSVFormat,
                Diligent::RefCntAutoPtr<Diligent::IShaderSourceInputStreamFactory> pShaderSourceFactory,
                const std::string& VSFilePath,
                const std::string& PSFilePath
                );

        Diligent::RefCntAutoPtr<Diligent::IBuffer> CreateVertexBuffer();
        Diligent::RefCntAutoPtr<Diligent::IBuffer> CreateIndexBuffer();
        void CreateInstanceBuffer();
        void CreateTextureAtlas();

        Diligent::RefCntAutoPtr<Diligent::IRenderDevice> m_pDevice;
        Diligent::RefCntAutoPtr<Diligent::IDeviceContext> m_pImmediateContext;

        Diligent::RefCntAutoPtr<Diligent::IPipelineState>         m_pPSO;
        Diligent::RefCntAutoPtr<Diligent::IBuffer>                m_QuadVertexBuffer;
        Diligent::RefCntAutoPtr<Diligent::IBuffer>                m_QuadIndexBuffer;
        Diligent::RefCntAutoPtr<Diligent::IBuffer>                m_InstanceBuffer;
        Diligent::RefCntAutoPtr<Diligent::IBuffer>                m_VSConstants;
        Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> m_SRB;
        Diligent::RefCntAutoPtr<Diligent::IDynamicTextureAtlas> m_pTextureAtlas;

        struct InstanceData
        {
                glm::mat4 Matrix;
                float TextureIndex;
        };

        glm::mat4 m_ViewProjMatrix;
        static constexpr int MaxInstances = 4096;
        static constexpr int MaxTextures = 4;
        std::array<InstanceData, MaxInstances> instance_data;
        std::array<InstanceData, MaxInstances>::iterator instance_data_iterator;
        std::array<Diligent::RefCntAutoPtr<Diligent::ITexture>, MaxTextures> texture_data;
        std::array<Diligent::RefCntAutoPtr<Diligent::ITexture>, MaxTextures>::iterator texture_data_iterator;
};
}