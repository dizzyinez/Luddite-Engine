#include "Luddite/Core/AssetTypes/Material.hpp"
#include "Luddite/Graphics/Renderer.hpp"
#include "Luddite/Core/Assets.hpp"
#include "Luddite/Core/Profiler.hpp"

namespace Luddite
{
void MaterialLibrary::Initialize()
{
        m_AssetBaseDir = "./Assets/Materials/";
        m_Extensions.emplace(L".ldmaterial");
}
Material* MaterialLibrary::LoadFromFile(const std::filesystem::path& path)
{
        std::stringstream ss;
        ss << "Loading Material: " << path;
        LD_PROFILE_SCOPE(ss.str());

        Material* mat = new Material();
        mat->m_Name = path.filename().generic_string();
        YAML::Node yaml = YAML::LoadFile(path.string());

        if (!yaml["Shader"])
        {
                LD_LOG_ERROR("Material \"{}\" doesn't specify a shader!", mat->m_Name);
                delete mat;
                return nullptr;
        }

        unsigned long long shader_id = yaml["Shader"].as<unsigned long long>();
        mat->m_pShader = Assets::GetShaderLibrary().GetAssetSynchronous(shader_id);
        const auto& PropDesc = mat->m_pShader->m_PropertiesBufferDescription;
        mat->m_Properties = PropDesc.CreateData(mat->m_Name);
        PropDesc.SetAttribsFromYaml(mat->m_Properties, yaml);

        mat->m_pShader.getCounter()->SetReloadFunction(mat, [ = ](void* caller, Shader* prev, Shader* curr) {
                        Material* mat = (Material*)caller;
                        LD_LOG_INFO("MATERIAL RELOAD FUNCTION");
                        //std::lock_guard lock{mat->m_Mutex};
                        mat->m_Properties = curr->m_PropertiesBufferDescription.CreateData(mat->m_Name);
                        PropDesc.SetAttribsFromYaml(mat->m_Properties, yaml);
                });
        //mat->SetReloadFunction(mat, [](void* caller, Material& prev, const Material& curr) {
        //                LD_LOG_INFO("RELOADING MATERIAL");
        //                prev.m_pShader->RemoveReloadFunction((void*)&prev);
        //        });
        // mat->m_pShader->m_Materials.push_back
        return mat;
}

Material::~Material()
{
        //LD_LOG_INFO("MATERIAL DESTRUCTION");
        if (m_pShader.valid())
                m_pShader.getCounter()->RemoveReloadFunction((void*)this);
}
}
