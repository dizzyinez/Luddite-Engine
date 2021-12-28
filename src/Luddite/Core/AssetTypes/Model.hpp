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
        Diligent::LayoutElement{4, 0, 4, Diligent::VT_UINT32, Diligent::False},  //BoneIndices
        Diligent::LayoutElement{5, 0, 4, Diligent::VT_FLOAT32, Diligent::False},  //BoneWeights
        Diligent::LayoutElement{6, 0, 4, Diligent::VT_FLOAT32, Diligent::False},  //Color
        Diligent::LayoutElement{7, 0, 2, Diligent::VT_FLOAT32, Diligent::False} //UV
};

// static_assert(sizeof(Vertex) % 16 == 0);

struct Bone
{
        glm::mat4 m_OffsetMatrix;
        unsigned int m_NodeID;
};

struct Node
{
        std::string m_Name;
        glm::mat4 m_Transform;
        int m_ParentNodeID;
        glm::vec3 m_Pos;
        glm::vec3 m_Rot;
        glm::vec3 m_Scale;
};

struct MeshInstance
{
        unsigned int m_MeshIndex;
        unsigned int m_NodeIndex;
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

struct PositionKey
{
        glm::vec3 m_Position;
        double m_TimeStamp;
};

struct RotationKey
{
        glm::quat m_Rotation;
        double m_TimeStamp;
};

struct ScaleKey
{
        glm::vec3 m_Scale;
        double m_TimeStamp;
};

struct AnimChannel
{
        unsigned int m_NodeID;
        std::vector<PositionKey> m_PositionKeys;
        std::vector<RotationKey> m_RotationKeys;
        std::vector<ScaleKey> m_ScaleKeys;
        inline glm::vec3 GetInterpolatedPosition(double time_stamp) const
        {
                LD_VERIFY(m_PositionKeys.size() > 0, "Animation has no position keys!");
                if (m_PositionKeys.size() == 1)
                        return m_PositionKeys.at(0).m_Position;
                return m_PositionKeys[0].m_Position;
                unsigned int idx = 0;
                while (idx < m_PositionKeys.size() - 1)
                {
                        if (time_stamp < m_PositionKeys[idx + 1].m_TimeStamp)
                                break;
                        idx++;
                }
                const auto& key1 = m_PositionKeys[idx];
                const auto& key2 = m_PositionKeys[idx + 1];
                const double dt = key2.m_TimeStamp - key1.m_TimeStamp;
                const double fac = (time_stamp - key1.m_TimeStamp) / dt;
                return glm::mix(key1.m_Position, key2.m_Position, fac);
        }

        inline glm::quat GetInterpolatedRotation(double time_stamp) const
        {
                LD_VERIFY(m_RotationKeys.size() > 0, "Animation has no rotation keys!");
                if (m_RotationKeys.size() == 1)
                        return glm::quat(m_RotationKeys[0].m_Rotation);
                unsigned int idx = 0;
                while (idx < m_RotationKeys.size() - 1)
                {
                        if (time_stamp < m_RotationKeys[idx + 1].m_TimeStamp)
                                break;
                        idx++;
                }
                const auto& key1 = m_RotationKeys[idx];
                const auto& key2 = m_RotationKeys[idx + 1];
                const double dt = key2.m_TimeStamp - key1.m_TimeStamp;
                const float fac = (time_stamp - key1.m_TimeStamp) / dt;
                return glm::slerp(glm::quat(key1.m_Rotation), glm::quat(key2.m_Rotation), fac);
        }

        inline glm::vec3 GetInterpolatedScale(double time_stamp) const
        {
                LD_VERIFY(m_ScaleKeys.size() > 0, "Animation has no scale keys!");
                if (m_ScaleKeys.size() == 1)
                        return m_ScaleKeys[0].m_Scale;
                unsigned int idx = 0;
                while (idx < m_ScaleKeys.size() - 1)
                {
                        if (time_stamp < m_ScaleKeys[idx + 1].m_TimeStamp)
                                break;
                        idx++;
                }
                const auto& key1 = m_ScaleKeys[idx];
                const auto& key2 = m_ScaleKeys[idx + 1];
                const double dt = key2.m_TimeStamp - key1.m_TimeStamp;
                const double fac = (time_stamp - key1.m_TimeStamp) / dt;
                return glm::mix(key1.m_Scale, key2.m_Scale, fac);
        }
};

struct Animation
{
        std::string m_Name;
        std::vector<AnimChannel> m_Channels;
        double m_TicksPerSecond;
        double m_Duration;
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
        std::vector<Animation> m_Animations;
        std::vector<Bone> m_Bones;
        std::unordered_map<std::string, unsigned int> m_BoneIDs;
        glm::mat4 m_GlobalInverseTransform;
};

class LUDDITE_API ModelLibrary : public AssetLibrary<Model, ModelLibrary>
{
        public:

        virtual void Initialize() override;
        virtual Model* LoadFromFile(const std::filesystem::path& path) override;
        virtual void AfterLoadProcessing(Model* pModel) override;
};
}
