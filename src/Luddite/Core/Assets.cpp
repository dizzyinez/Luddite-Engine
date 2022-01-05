#include "Luddite/Core/Assets.hpp"
#include "Luddite/Core/Profiler.hpp"
namespace Luddite
{
ModelLibrary Assets::m_ModelLibrary;
TextureLibrary Assets::m_TextureLibrary;
ShaderLibrary Assets::m_ShaderLibrary;
MaterialLibrary Assets::m_MaterialLibrary;

void Assets::Initialize()
{
        LD_PROFILE_FUNCTION();
        m_ModelLibrary.Initialize();
        m_ModelLibrary.InitializeFiles();

        m_ShaderLibrary.Initialize();
        m_ShaderLibrary.InitializeFiles();

        m_MaterialLibrary.Initialize();
        m_MaterialLibrary.InitializeFiles();

        m_TextureLibrary.Initialize();
        m_TextureLibrary.InitializeFiles();
}

void Assets::RefreshAssets()
{
        ZoneScoped;
        LD_PROFILE_FUNCTION();
        m_ModelLibrary.RefreshAssetsFromFilesystem();
        m_ShaderLibrary.RefreshAssetsFromFilesystem();
        m_MaterialLibrary.RefreshAssetsFromFilesystem();
        m_TextureLibrary.RefreshAssetsFromFilesystem();
}

void Assets::MergeLoadedAssets()
{
        ZoneScoped;
        LD_PROFILE_FUNCTION();
        m_ModelLibrary.MergeLoadedAssets();
        m_ShaderLibrary.MergeLoadedAssets();
        m_MaterialLibrary.MergeLoadedAssets();
        m_TextureLibrary.MergeLoadedAssets();
}
}
