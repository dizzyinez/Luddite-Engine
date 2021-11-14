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

namespace Luddite
{
void ModelLibrary::Initialize()
{
        m_AssetBaseDir = "./Assets/Models/";
        m_Extensions.push_back(L".obj");
        m_Extensions.push_back(L".blend");
        m_Extensions.push_back(L".fbx");
}

void ProcessNode(aiNode* node, const aiScene* scene)
{
}

void ProcessMesh()
{
}

void AddNode(aiNode* node, int parent_node_id, unsigned int& node_id_counter, Model* model) {
        model->m_Nodes.push_back({
                        glm::transpose(*reinterpret_cast<glm::mat4*>(&node->mTransformation)),
                        parent_node_id
                });
        unsigned int this_node_id = node_id_counter;
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
                model->m_MeshNodePairs.push_back({node->mMeshes[i], this_node_id});
        node_id_counter++;
        for (unsigned int i = 0; i < node->mNumChildren; i++)
                AddNode(node->mChildren[i], this_node_id, node_id_counter, model);
};

Model* ModelLibrary::LoadFromFile(const std::filesystem::path& path)
{
        std::stringstream ss;
        ss << "Loading Model: " << path;
        LD_PROFILE_SCOPE(ss.str());

        Assimp::Importer importer;
        const aiScene* scene =
                importer.ReadFile(path.string(),
                        aiProcess_GenNormals |
                        aiProcess_GenBoundingBoxes |
                        aiProcess_GenUVCoords |
                        aiProcess_Triangulate |
                        aiProcess_JoinIdenticalVertices |
                        aiProcess_SortByPType |
                        aiProcess_MakeLeftHanded |
                        aiProcess_CalcTangentSpace
                        );
        importer.ApplyPostProcessing(aiProcess_CalcTangentSpace);
        LD_VERIFY(scene != nullptr, "Error loading model: {}", importer.GetErrorString());

        Model* model = new Model();
        model->m_Name = scene->mName.C_Str();
        model->m_Meshes.reserve(scene->mNumMeshes);

        model->m_Materials.reserve(scene->mNumMaterials);
        auto mat = Assets::GetMaterialLibrary().GetAsset(8208622817716570807ULL);
        for (int m = 0; m < scene->mNumMaterials; m++)
                model->m_Materials.push_back(mat);


        // scene->mAnimations[0]
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
                        vertex.Position = *reinterpret_cast<glm::vec3*>(&ai_mesh.mVertices[v]);
                        vertex.Normal = *reinterpret_cast<glm::vec3*>(&ai_mesh.mNormals[v]);
                        if (ai_mesh.mTangents)
                                vertex.Tangent = *reinterpret_cast<glm::vec3*>(&ai_mesh.mTangents[v]);
                        else
                                vertex.Tangent = glm::vec3(1.f, 0.f, 0.f);
                        if (ai_mesh.mBitangents)
                                vertex.Bitangent = *reinterpret_cast<glm::vec3*>(&ai_mesh.mBitangents[v]);
                        else
                                vertex.Bitangent = glm::vec3(0.f, 1.f, 0.f);
                        if (ai_mesh.mColors[0])
                                vertex.Color = *reinterpret_cast<glm::vec4*>(&ai_mesh.mColors[0][v]);
                        else
                                vertex.Color = glm::vec4(1.f);
                        if (ai_mesh.mTextureCoords[0])
                                vertex.UV = *reinterpret_cast<glm::vec2*>(&ai_mesh.mTextureCoords[0][v]);
                        else
                                vertex.UV = glm::vec2(0.f);
                }

                mesh.m_nIndicies = ai_mesh.mNumFaces * 3;
                uint32_t* indices = new uint32_t[ai_mesh.mNumFaces * 3];
                for (int i = 0; i < ai_mesh.mNumFaces; i++)
                {
                        indices[(i * 3) + 0] = ai_mesh.mFaces[i].mIndices[0];
                        indices[(i * 3) + 1] = ai_mesh.mFaces[i].mIndices[1];
                        indices[(i * 3) + 2] = ai_mesh.mFaces[i].mIndices[2];
                }
                //TODO: bones

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

        unsigned int node_id_counter = 0;
        AddNode(scene->mRootNode, -1, node_id_counter, model);
        // for (int n = 0; n < scene->mRootNode->)
        //         AllocateBuffers(model);
        return model;
}

void ModelLibrary::AllocateBuffers(Model* model)
{
        // for (auto& mesh : model->meshes)
        // {
        // }
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
