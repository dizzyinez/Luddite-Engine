#include "Luddite/Graphics/ModelLoader.hpp"
#include "Luddite/Graphics/MaterialLibrary.hpp"

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
        return model;
}
void ModelLoader::BasicModelAllocator::LoadOBJ(std::shared_ptr<BasicModel> model, std::string filepath)
{
        objl::Loader loader;
        loader.LoadFile(filepath);

        //load materials
        std::vector<Material> materials;
        materials.reserve(loader.LoadedMaterials.size());
        for (auto obj_material : loader.LoadedMaterials)
        {
                MaterialDescription MatDesc;
                MatDesc.Name = obj_material.name;
                MatDesc.Diffuse = ColorRGB(*(glm::vec3*)&obj_material.Kd);
                MatDesc.Specular = ColorRGB(*(glm::vec3*)&obj_material.Ks);
                // MatDesc.
                if (!obj_material.map_Kd.empty())
                {
                        //Load Texture
                        //use texture
                }
        }

        //load vertecies and indicies
        for (auto obj_mesh : loader.LoadedMeshes)
        {
                BasicMesh temp_mesh;
                temp_mesh.name = obj_mesh.MeshName;
                temp_mesh.vertecies.reserve(obj_mesh.Vertices.size());
                for (auto obj_vertex : obj_mesh.Vertices)
                {
                        temp_mesh.vertecies.push_back({
                                        *(glm::vec3*)&obj_vertex.Position,
                                        *(glm::vec3*)&obj_vertex.Normal,
                                        *(glm::vec2*)&obj_vertex.TextureCoordinate,
                                });
                }
                temp_mesh.indicies.reserve(obj_mesh.Indices.size());
                for (auto obj_index : obj_mesh.Indices)
                {
                        temp_mesh.indicies.push_back(obj_index);
                }
                model->meshes.push_back(temp_mesh);
        }
        LD_LOG_INFO("OBJ Loaded: {}", filepath);
}
}