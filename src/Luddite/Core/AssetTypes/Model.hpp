#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/Core/Core.hpp"
#include "Luddite/Core/AssetLibrary.hpp"
#include "Luddite/Graphics/DiligentInclude.hpp"
#include "Luddite/Core/AssetTypes/Material.hpp"

namespace Luddite
{
// struct LUDDITE_API BasicVertex
// {
//         glm::vec3 Position;
// };

struct Vertex
{
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec3 Tangent;
        glm::vec3 Bitangent;
        glm::uvec4 BoneIndicies;
        glm::vec4 BoneWeights;
        glm::vec4 Color;
        glm::vec2 UV;
};
const Diligent::LayoutElement VertexLayoutElements[] =
{
        Diligent::LayoutElement{0, 0, 3, Diligent::VT_FLOAT32, Diligent::False}, //Position
        Diligent::LayoutElement{1, 0, 3, Diligent::VT_FLOAT32, Diligent::False}, //Normal
        Diligent::LayoutElement{2, 0, 3, Diligent::VT_FLOAT32, Diligent::False}, //Tangent
        Diligent::LayoutElement{3, 0, 3, Diligent::VT_FLOAT32, Diligent::False}, //Bitangent
        Diligent::LayoutElement{5, 0, 4, Diligent::VT_UINT32, Diligent::False},  //BoneIndicies
        Diligent::LayoutElement{6, 0, 4, Diligent::VT_FLOAT32, Diligent::False},  //BoneWeights
        Diligent::LayoutElement{7, 0, 4, Diligent::VT_FLOAT32, Diligent::False},  //Color
        Diligent::LayoutElement{8, 0, 2, Diligent::VT_FLOAT32, Diligent::False} //UV
};

// static_assert(sizeof(Vertex) % 16 == 0);

struct Bone
{
};

struct Node
{
        glm::mat4 m_Transform;
        int m_ParentNodeID;
};

struct MeshInstance
{
        int m_MeshIndex;
        int m_NodeIndex;
};

struct Mesh
{
        std::string m_Name;
        // Vertex* m_Vertecies;
        int m_nVertices;
        // uint32_t* m_Indicies;
        int m_nIndicies;
        // Bone* m_Bones;
        int m_nBones;
        int m_MaterialIndex;
        Diligent::RefCntAutoPtr<Diligent::IBuffer> m_pVertexBuffer;
        Diligent::RefCntAutoPtr<Diligent::IBuffer> m_pIndexBuffer;
        // MaterialHandle material;
};

struct LUDDITE_API Model
{
        // Model(const std::string& name_) : m_Name(name_) {};
        // ~Model() = default;
        std::string m_Name;
        std::vector<Mesh> m_Meshes;
        std::vector<Node> m_Nodes;
        std::vector<MeshInstance> m_MeshNodePairs;
        std::vector<Handle<Material> > m_Materials;
        // Mesh* m_Meshes;
        // uint32_t m_nMeshes;
};

class LUDDITE_API ModelLibrary : public AssetLibrary<Model, ModelLibrary>
{
        public:

        virtual void Initialize() override;
        virtual Model* LoadFromFile(const std::filesystem::path& path) override;
        virtual void AfterLoadProcessing(Model* pModel) override;
        private:
        void AllocateBuffers(Model* model);
};
}
