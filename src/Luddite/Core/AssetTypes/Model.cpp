#include "Luddite/Core/AssetTypes/Model.hpp"
#include "Luddite/Graphics/Renderer.hpp"
#include "Luddite/Core/Profiler.hpp"

#include "Luddite/Core/Assets.hpp"

#ifdef LD_DEBUG
    #define OBJL_CONSOLE_OUTPUT
#endif // LD_DEBUG

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/assimp_glm_helpers.h
class AssimpGLMHelpers
{
        public:

        static inline glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4& from)
        {
                glm::mat4 to;
                //the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
                to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
                to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
                to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
                to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
                return to;
        }

        static inline glm::vec3 GetGLMVec(const aiColor3D& vec)
        {
                return glm::vec3(vec.r, vec.g, vec.b);
        }

        static inline glm::vec4 GetGLMVec(const aiColor4D& vec)
        {
                return glm::vec4(vec.r, vec.g, vec.b, vec.a);
        }

        static inline glm::vec3 GetGLMVec(const aiVector3D& vec)
        {
                return glm::vec3(vec.x, vec.y, vec.z);
        }

        static inline glm::vec2 GetGLMVec(const aiVector2D& vec)
        {
                return glm::vec2(vec.x, vec.y);
        }

        static inline glm::quat GetGLMQuat(const aiQuaternion& pOrientation)
        {
                return glm::quat(pOrientation.w, pOrientation.x, pOrientation.y, pOrientation.z);
        }
};

namespace Luddite
{
void ModelLibrary::Initialize()
{
        m_AssetBaseDir = "./Assets/Models/";
        m_Extensions.push_back(L".obj");
        m_Extensions.push_back(L".blend");
        m_Extensions.push_back(L".fbx");
        m_Extensions.push_back(L".gltf");
        m_Extensions.push_back(L".dae");
}

void ProcessNode(aiNode* node, const aiScene* scene)
{
}

void ProcessMesh()
{
}

void AddNode(aiNode* node, int parent_node_id, unsigned int& node_id_counter, Model* model, std::unordered_map<std::string, unsigned int >& id_map) {
        glm::mat4 transform = AssimpGLMHelpers::ConvertMatrixToGLMFormat(node->mTransformation);
        glm::vec3 scale;
        glm::quat rotation;
        glm::vec3 translation;
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::decompose(transform, scale, rotation, translation, skew, perspective);
        model->m_Nodes.push_back({
                        node->mName.C_Str(),
                        AssimpGLMHelpers::ConvertMatrixToGLMFormat(node->mTransformation),
                        parent_node_id,
                        translation,
                        glm::eulerAngles(rotation),
                        scale
                });
        unsigned int this_node_id = node_id_counter;
        id_map[node->mName.C_Str()] = this_node_id;
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
                model->m_MeshNodePairs.push_back({node->mMeshes[i], this_node_id});
        node_id_counter++;
        for (unsigned int i = 0; i < node->mNumChildren; i++)
                AddNode(node->mChildren[i], this_node_id, node_id_counter, model, id_map);
};


#ifndef LD_DEFAULT_ANIMATION_TPS
#define LD_DEFAULT_ANIMATION_TPS 25.
#endif
inline void LoadAnimations(const aiScene* scene, Model* model, std::unordered_map<std::string, unsigned int> id_map)
{
        model->m_Animations.reserve(scene->mNumAnimations);
        for (int a = 0; a < scene->mNumAnimations; a++)
        {
                auto& ai_animation = scene->mAnimations[a];
                auto& animation = model->m_Animations.emplace_back();
                animation.m_Name = ai_animation->mName.C_Str();
                animation.m_Channels.reserve(ai_animation->mNumChannels);
                animation.m_TicksPerSecond = ai_animation->mTicksPerSecond != 0 ? ai_animation->mTicksPerSecond : LD_DEFAULT_ANIMATION_TPS;
                animation.m_Duration = ai_animation->mDuration;
                for (int c = 0; c < ai_animation->mNumChannels; c++)
                {
                        auto& ai_channel = ai_animation->mChannels[c];
                        auto& channel = animation.m_Channels.emplace_back();

                        unsigned int node_id = id_map.at(ai_channel->mNodeName.C_Str());
                        LD_VERIFY(node_id > 0 && node_id < model->m_Nodes.size(), "Node ID map not built correctly!");
                        LD_VERIFY(model->m_Nodes[node_id].m_Name == ai_channel->mNodeName.C_Str(), "Node ID map not built correctly!");
                        channel.m_NodeID = node_id;
                        auto& node = model->m_Nodes[node_id];

                        channel.m_PositionKeys.reserve(ai_channel->mNumPositionKeys);
                        for (int i = 0; i < ai_channel->mNumPositionKeys; i++)
                        {
                                auto& pos = ai_channel->mPositionKeys[i].mValue;
                                channel.m_PositionKeys.emplace_back(
                                        AssimpGLMHelpers::GetGLMVec(pos),
                                        ai_channel->mPositionKeys[i].mTime
                                        );
                        }

                        channel.m_RotationKeys.reserve(ai_channel->mNumRotationKeys);
                        for (int i = 0; i < ai_channel->mNumRotationKeys; i++)
                        {
                                auto& rot = ai_channel->mRotationKeys[i].mValue;
                                channel.m_RotationKeys.emplace_back(
                                        AssimpGLMHelpers::GetGLMQuat(rot),
                                        ai_channel->mRotationKeys[i].mTime
                                        );
                        }

                        channel.m_ScaleKeys.reserve(ai_channel->mNumScalingKeys);
                        for (int i = 0; i < ai_channel->mNumScalingKeys; i++)
                        {
                                auto& scale = ai_channel->mScalingKeys[i].mValue;
                                channel.m_ScaleKeys.emplace_back(
                                        AssimpGLMHelpers::GetGLMVec(scale),
                                        ai_channel->mScalingKeys[i].mTime
                                        );
                        }
                }
        }
}

inline void LoadMeshes(const aiScene* scene, Model* model, std::unordered_map<std::string, unsigned int> id_map)
{
        model->m_Meshes.reserve(scene->mNumMeshes);
        for (int m = 0; m < scene->mNumMeshes; m++)
        {
                Diligent::BufferDesc VertBuffDesc;
                VertBuffDesc.Usage = Diligent::USAGE_IMMUTABLE;
                VertBuffDesc.BindFlags = Diligent::BIND_VERTEX_BUFFER;
                Diligent::BufferDesc IndBuffDesc;
                IndBuffDesc.Usage = Diligent::USAGE_IMMUTABLE;
                IndBuffDesc.BindFlags = Diligent::BIND_INDEX_BUFFER;

                auto& ai_mesh = *scene->mMeshes[m];
                auto& mesh = model->m_Meshes.emplace_back();
                mesh.m_Name = ai_mesh.mName.C_Str();
                mesh.m_MaterialIndex = ai_mesh.mMaterialIndex;
                mesh.m_nVertices = ai_mesh.mNumVertices;
                Vertex* vertices = new Vertex[ai_mesh.mNumVertices];

                if (!ai_mesh.HasTangentsAndBitangents())
                        LD_LOG_WARN("Model: {}, Mesh {} doesn't have/didn't generate tangents and bitangents!", model->m_Name, mesh.m_Name);
                for (int v = 0; v < ai_mesh.mNumVertices; v++)
                {
                        auto& vertex = vertices[v];
                        vertex.Position = AssimpGLMHelpers::GetGLMVec(ai_mesh.mVertices[v]);
                        vertex.Normal = AssimpGLMHelpers::GetGLMVec(ai_mesh.mNormals[v]);
                        if (ai_mesh.mTangents)
                                vertex.Tangent = AssimpGLMHelpers::GetGLMVec(ai_mesh.mTangents[v]);
                        else
                                vertex.Tangent = glm::vec3(1.f, 0.f, 0.f);
                        if (ai_mesh.mBitangents)
                                vertex.Bitangent = AssimpGLMHelpers::GetGLMVec(ai_mesh.mBitangents[v]);
                        else
                                vertex.Bitangent = glm::vec3(0.f, 1.f, 0.f);
                        if (ai_mesh.mColors[0])
                                vertex.Color = AssimpGLMHelpers::GetGLMVec(ai_mesh.mColors[0][v]);
                        else
                                vertex.Color = glm::vec4(1.f);
                        if (ai_mesh.mTextureCoords[0])
                                vertex.UV = AssimpGLMHelpers::GetGLMVec(ai_mesh.mTextureCoords[0][v]);
                        else
                                vertex.UV = glm::vec2(0.f);
                        vertex.BoneIndicies = glm::uvec4(0);
                        vertex.BoneWeights = glm::vec4(0.0);
                }

                mesh.m_nIndicies = ai_mesh.mNumFaces * 3;
                uint32_t* indices = new uint32_t[ai_mesh.mNumFaces * 3];
                for (int i = 0; i < ai_mesh.mNumFaces; i++)
                {
                        indices[(i * 3) + 0] = ai_mesh.mFaces[i].mIndices[0];
                        indices[(i * 3) + 1] = ai_mesh.mFaces[i].mIndices[1];
                        indices[(i * 3) + 2] = ai_mesh.mFaces[i].mIndices[2];
                }

                if (ai_mesh.HasBones())
                {
                        for (int b = 0; b < ai_mesh.mNumBones; b++)
                        {
                                auto& ai_bone = ai_mesh.mBones[b];

                                unsigned int idx = 0;
                                if (model->m_BoneIDs.find(ai_bone->mName.C_Str()) == model->m_BoneIDs.end())
                                {
                                        idx = model->m_BoneIDs.size();
                                        model->m_BoneIDs[ai_bone->mName.C_Str()] = idx;
                                        model->m_Bones.emplace_back();
                                }
                                else
                                        idx = model->m_BoneIDs[ai_bone->mName.C_Str()];
                                auto& bone = model->m_Bones[idx];
                                bone.m_OffsetMatrix = AssimpGLMHelpers::ConvertMatrixToGLMFormat(ai_bone->mOffsetMatrix);
                                bone.m_NodeID = id_map[ai_bone->mName.C_Str()];
                                for (int w = 0; w < ai_bone->mNumWeights; w++)
                                {
                                        auto& ai_weight = ai_bone->mWeights[w];
                                        auto& vertex = vertices[ai_weight.mVertexId];
                                        for (int i = 0; i < 4; i++)
                                        {
                                                if (vertex.BoneWeights[i] == 0.0)
                                                {
                                                        vertex.BoneWeights[i] = ai_weight.mWeight;
                                                        vertex.BoneIndicies[i] = idx;
                                                        break;
                                                }
                                        }
                                }
                        }
                }

                //create vbo
                VertBuffDesc.Name = mesh.m_Name.c_str();
                VertBuffDesc.uiSizeInBytes = mesh.m_nVertices * sizeof(Vertex);
                Diligent::BufferData VBData;
                VBData.pData = reinterpret_cast<void*>(vertices);
                VBData.DataSize = VertBuffDesc.uiSizeInBytes;
                Renderer::GetDevice()->CreateBuffer(VertBuffDesc, &VBData, &mesh.m_pVertexBuffer);

                //create ibo
                IndBuffDesc.Name = mesh.m_Name.c_str();
                IndBuffDesc.uiSizeInBytes = mesh.m_nIndicies * sizeof(uint32_t);
                Diligent::BufferData IBData;
                IBData.pData = reinterpret_cast<void*>(indices);
                IBData.DataSize = IndBuffDesc.uiSizeInBytes;
                Renderer::GetDevice()->CreateBuffer(IndBuffDesc, &IBData, &mesh.m_pIndexBuffer);
                delete [] vertices;
                delete [] indices;
        }
};

Model* ModelLibrary::LoadFromFile(const std::filesystem::path& path)
{
        std::stringstream ss;
        ss << "Loading Model: " << path;
        LD_PROFILE_SCOPE(ss.str());

        Assimp::Importer importer;
        //importer.
        const aiScene* scene =
                importer.ReadFile(path.string(),
                        aiProcess_GenNormals |
                        aiProcess_GenBoundingBoxes |
                        aiProcess_GenUVCoords |
                        aiProcess_Triangulate |
                        aiProcess_JoinIdenticalVertices |
                        aiProcess_SortByPType |
                        aiProcess_MakeLeftHanded |
                        //aiProcess_FlipUVs |
                        aiProcess_LimitBoneWeights |
                        aiProcess_CalcTangentSpace
                        );
        importer.ApplyPostProcessing(aiProcess_CalcTangentSpace);
        LD_VERIFY(scene != nullptr, "Error loading model: {}", importer.GetErrorString());

        Model* model = new Model();
        model->m_Name = scene->mName.C_Str();
        model->m_GlobalInverseTransform = glm::inverse(AssimpGLMHelpers::ConvertMatrixToGLMFormat(scene->mRootNode->mTransformation));

        model->m_Materials.reserve(scene->mNumMaterials);
        auto mat = Assets::GetMaterialLibrary().GetAsset(8208622817716570807ULL);
        for (int m = 0; m < scene->mNumMaterials; m++)
                model->m_Materials.push_back(mat);

        unsigned int node_id_counter = 0;
        std::unordered_map<std::string, unsigned int> id_map;
        AddNode(scene->mRootNode, -1, node_id_counter, model, id_map);

        LoadMeshes(scene, model, id_map);
        LoadAnimations(scene, model, id_map);

        importer.FreeScene();


        return model;
}

void ModelLibrary::AfterLoadProcessing(Model* pModel)
{
        LD_PROFILE_FUNCTION();
        for (auto& mesh : pModel->m_Meshes)
        {
                //transition resources
                Diligent::StateTransitionDesc Barriers[] =
                {
                        {mesh.m_pVertexBuffer, Diligent::RESOURCE_STATE_UNKNOWN, Diligent::RESOURCE_STATE_VERTEX_BUFFER, true},
                        {mesh.m_pIndexBuffer, Diligent::RESOURCE_STATE_UNKNOWN, Diligent::RESOURCE_STATE_INDEX_BUFFER, true}
                };

                Renderer::GetContext()->TransitionResourceStates(_countof(Barriers), Barriers);
        }
}
}
