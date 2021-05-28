#include "Luddite/Graphics/ModelLibrary.hpp"
#include "Luddite/Graphics/MaterialLibrary.hpp"
#include "Luddite/Graphics/Renderer.hpp"

#ifdef LD_DEBUG
    #define OBJL_CONSOLE_OUTPUT
#endif // LD_DEBUG

#define TINYOBJLOADER_IMPLEMENTATION
#include "OBJ_Loader.h"

namespace Luddite
{
std::shared_ptr<BasicModel> ModelLoader::GetBasicModel(const std::string& filepath)
{
        return m_ModelAllocator.Get(filepath);
}

std::shared_ptr<BasicModel> ModelLoader::BasicModelAllocator::Allocate(const std::string& filepath)
{
        std::string name;
        auto last_slash = filepath.find_last_of("/");
        if (last_slash != std::string::npos)
                name = filepath.substr(last_slash + 1, filepath.find_last_of(".") - last_slash - 1);
        else
                name = filepath.substr(0, filepath.find_last_of("."));

        std::shared_ptr<BasicModel> model = std::make_shared<BasicModel>();
        model->name = name;

        std::string extension = filepath.substr(filepath.find_last_of(".") + 1);
        if (extension == "obj")
                LoadOBJ(model, filepath);

        CreateBuffers(model);

        return model;
}

void ModelLoader::BasicModelAllocator::CreateBuffers(std::shared_ptr<BasicModel> model)
{
        for (auto mesh : model->meshes)
        {
                //create vbo
                Diligent::BufferDesc VertBuffDesc;
                VertBuffDesc.Name = mesh->name.c_str();
                VertBuffDesc.Usage = Diligent::USAGE_IMMUTABLE;
                VertBuffDesc.BindFlags = Diligent::BIND_VERTEX_BUFFER;
                VertBuffDesc.uiSizeInBytes = mesh->vertecies.size() * sizeof(BasicVertex);
                Diligent::BufferData VBData;
                VBData.pData = mesh->vertecies.data();
                VBData.DataSize = mesh->vertecies.size() * sizeof(BasicVertex);
                Renderer::GetDevice()->CreateBuffer(VertBuffDesc, &VBData, &mesh->m_pVertexBuffer);

                //create ibo
                Diligent::BufferDesc IndBuffDesc;
                IndBuffDesc.Name = mesh->name.c_str();
                IndBuffDesc.Usage = Diligent::USAGE_IMMUTABLE;
                IndBuffDesc.BindFlags = Diligent::BIND_INDEX_BUFFER;
                IndBuffDesc.uiSizeInBytes = mesh->indicies.size() * sizeof(uint32_t);
                Diligent::BufferData IBData;
                IBData.pData = mesh->indicies.data();
                IBData.DataSize = mesh->indicies.size() * sizeof(uint32_t);
                Renderer::GetDevice()->CreateBuffer(IndBuffDesc, &IBData, &mesh->m_pIndexBuffer);

                //transition resources
                Diligent::StateTransitionDesc Barriers[] =
                {
                        {mesh->m_pVertexBuffer, Diligent::RESOURCE_STATE_UNKNOWN, Diligent::RESOURCE_STATE_VERTEX_BUFFER, true},
                        {mesh->m_pIndexBuffer, Diligent::RESOURCE_STATE_UNKNOWN, Diligent::RESOURCE_STATE_INDEX_BUFFER, true}
                };

                Renderer::GetContext()->TransitionResourceStates(_countof(Barriers), Barriers);
        }
}

void ModelLoader::BasicModelAllocator::LoadOBJ(std::shared_ptr<BasicModel> model, std::string filepath)
{
        objl::Loader loader;
        loader.LoadFile(filepath);

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
                BasicMeshHandle temp_mesh = std::make_shared<BasicMesh>();
                temp_mesh->name = obj_mesh.MeshName;
                temp_mesh->vertecies.reserve(obj_mesh.Vertices.size());
                for (auto obj_vertex : obj_mesh.Vertices)
                {
                        temp_mesh->vertecies.push_back({
                                        *(glm::vec3*)&obj_vertex.Position,
                                        *(glm::vec3*)&obj_vertex.Normal,
                                        *(glm::vec2*)&obj_vertex.TextureCoordinate,
                                });
                }
                temp_mesh->indicies.reserve(obj_mesh.Indices.size());
                //put indicies in reverse order to correct normal directions
                for (auto rit = obj_mesh.Indices.rbegin(); rit != obj_mesh.Indices.rend(); ++rit)
                {
                        temp_mesh->indicies.push_back(*rit);
                }
                model->meshes.emplace_back(temp_mesh);
        }
        LD_LOG_INFO("OBJ Loaded: {}", filepath);
}
}