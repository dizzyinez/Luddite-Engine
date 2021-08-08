#include "Luddite/Core/Assets.hpp"

namespace Luddite
{
BasicModelLibrary Assets::m_BasicModelLibrary;
TextureLibrary Assets::m_TextureLibrary;
ShaderLibrary Assets::m_ShaderLibrary;

void Assets::Initialize()
{
        m_BasicModelLibrary.Initialize();
        m_BasicModelLibrary.InitializeFiles();

        m_ShaderLibrary.Initialize();
        m_ShaderLibrary.InitializeFiles();
}

void Assets::RefreshAssets()
{
        m_BasicModelLibrary.RefreshAssetsFromFilesystem();
        m_ShaderLibrary.RefreshAssetsFromFilesystem();
}

void Assets::MergeLoadedAssets()
{
        m_BasicModelLibrary.MergeLoadedAssets();
        m_ShaderLibrary.MergeLoadedAssets();
}
}