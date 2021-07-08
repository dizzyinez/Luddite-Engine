#include "Luddite/Core/AssetTypes/Model.hpp"
#include "Luddite/Graphics/Renderer.hpp"

#ifdef LD_DEBUG
    #define OBJL_CONSOLE_OUTPUT
#endif // LD_DEBUG

#define TINYOBJLOADER_IMPLEMENTATION
#include "OBJ_Loader.h"

namespace Luddite
{
void BasicModelLibrary::Initialize()
{
        m_AssetBaseDir = "./Assets/Models/";
        m_Extensions.push_back(L".obj");
}


BasicModel* BasicModelLibrary::LoadFromFile(const std::filesystem::path& path)
{
        auto extension = path.extension().string();
        if (extension == ".obj")
        {
                BasicModel* model = LoadOBJ(path);
                AllocateBuffers(model);
                return model;
        }
}

void BasicModelLibrary::AllocateBuffers(BasicModel* model)
{
        for (auto& mesh : model->meshes)
        {
                //create vbo
                Diligent::BufferDesc VertBuffDesc;
                VertBuffDesc.Name = mesh.name.c_str();
                VertBuffDesc.Usage = Diligent::USAGE_IMMUTABLE;
                VertBuffDesc.BindFlags = Diligent::BIND_VERTEX_BUFFER;
                VertBuffDesc.uiSizeInBytes = mesh.vertecies.size() * sizeof(BasicVertex);
                Diligent::BufferData VBData;
                VBData.pData = mesh.vertecies.data();
                VBData.DataSize = mesh.vertecies.size() * sizeof(BasicVertex);
                Renderer::GetDevice()->CreateBuffer(VertBuffDesc, &VBData, &mesh.m_pVertexBuffer);

                //create ibo
                Diligent::BufferDesc IndBuffDesc;
                IndBuffDesc.Name = mesh.name.c_str();
                IndBuffDesc.Usage = Diligent::USAGE_IMMUTABLE;
                IndBuffDesc.BindFlags = Diligent::BIND_INDEX_BUFFER;
                IndBuffDesc.uiSizeInBytes = mesh.indicies.size() * sizeof(uint32_t);
                Diligent::BufferData IBData;
                IBData.pData = mesh.indicies.data();
                IBData.DataSize = mesh.indicies.size() * sizeof(uint32_t);
                Renderer::GetDevice()->CreateBuffer(IndBuffDesc, &IBData, &mesh.m_pIndexBuffer);
        }
}

void BasicModelLibrary::AfterLoadProcessing(BasicModel* pModel)
{
        for (auto& mesh : pModel->meshes)
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
BasicModel* BasicModelLibrary::LoadOBJ(const std::filesystem::path& path)
{
        objl::Loader loader;
        loader.LoadFile(path.string());

        BasicModel* model = new BasicModel();

        //load materials
        // std::vector<MaterialHandle> materials;
        // materials.reserve(loader.LoadedMaterials.size());
        // for (auto obj_material : loader.LoadedMaterials)
        // {
        //         MaterialDescription MatDesc;
        //         MatDesc.Name = obj_material.name;
        //         MatDesc.Diffuse = ColorRGB(*(glm::vec3*)&obj_material.Kd);
        //         MatDesc.Specular = ColorRGB(*(glm::vec3*)&obj_material.Ks);
        //         // MatDesc.
        //         if (!obj_material.map_Kd.empty())
        //         {
        //                 //Load Texture
        //                 //use texture
        //         }
        // }

        //load vertecies and indicies
        for (auto obj_mesh : loader.LoadedMeshes)
        {
                BasicMesh& mesh = model->meshes.emplace_back();
                mesh.name = obj_mesh.MeshName;
                mesh.vertecies.reserve(obj_mesh.Vertices.size());
                for (auto obj_vertex : obj_mesh.Vertices)
                {
                        mesh.vertecies.push_back({
                                        *(glm::vec3*)&obj_vertex.Position,
                                        *(glm::vec3*)&obj_vertex.Normal,
                                        *(glm::vec2*)&obj_vertex.TextureCoordinate,
                                });
                }
                mesh.indicies.reserve(obj_mesh.Indices.size());
                //put indicies in reverse order to correct normal directions
                for (auto rit = obj_mesh.Indices.rbegin(); rit != obj_mesh.Indices.rend(); ++rit)
                {
                        mesh.indicies.push_back(*rit);
                }
                // model->meshes.emplace_back(mesh);
        }
        return model;
}
}