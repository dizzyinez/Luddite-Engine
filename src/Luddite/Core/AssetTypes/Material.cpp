#include "Luddite/Core/AssetTypes/Material.hpp"
#include "Luddite/Graphics/Renderer.hpp"
#include "Luddite/Core/Assets.hpp"

namespace Luddite
{
void MaterialLibrary::Initialize()
{
        m_AssetBaseDir = "./Assets/Materials/";
        m_Extensions.push_back(L".ldmaterial");
}
Material* MaterialLibrary::LoadFromFile(const std::filesystem::path& path)
{
        Material* mat = new Material();
        mat->m_pShader = Assets::GetShaderLibrary().GetAssetSynchronous(14332508749617699857ULL);
        mat->m_Name = path.filename().generic_string();
        mat->m_Properties = mat->m_pShader->m_PropertiesBufferDescription.CreateData(mat->m_Name);
        mat->m_pShader.getCounter()->SetReloadFunction(mat, [](void* caller, Shader* prev, Shader* curr) {
                        Material* mat = (Material*)caller;
                        LD_LOG_INFO("MATERIAL RELOAD FUNCTION");
                        //std::lock_guard lock{mat->m_Mutex};
                        mat->m_Properties = curr->m_PropertiesBufferDescription.CreateData(mat->m_Name);
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
