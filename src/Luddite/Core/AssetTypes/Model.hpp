#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/Core/Core.hpp"
#include "Luddite/Core/AssetLibrary.hpp"
#include "Luddite/Graphics/Material.hpp"

namespace Luddite
{
struct LUDDITE_API BasicVertex
{
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 UV;
};

struct LUDDITE_API BasicMesh
{
        std::string name;
        std::vector<BasicVertex> vertecies;
        std::vector<uint32_t> indicies;
        Diligent::RefCntAutoPtr<Diligent::IBuffer> m_pVertexBuffer;
        Diligent::RefCntAutoPtr<Diligent::IBuffer> m_pIndexBuffer;
        // MaterialHandle material;
};

struct LUDDITE_API BasicModel : public Asset<BasicModel>
{
        std::string name;
        std::vector<BasicMesh> meshes;
};

class LUDDITE_API BasicModelLibrary : public AssetLibrary<BasicModel, BasicModelLibrary>
{
        public:

        virtual void Initialize() override;
        virtual BasicModel* LoadFromFile(const std::filesystem::path& path) override;
        virtual void AfterLoadProcessing(BasicModel* pModel) override;
        private:
        BasicModel* LoadOBJ(const std::filesystem::path& path);
        void AllocateBuffers(BasicModel* model);
};
}