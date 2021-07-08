#include "Luddite/Core/Assets.hpp"

namespace Luddite
{
BasicModelLibrary Assets::m_BasicModelLibrary;

void Assets::Initialize()
{
        m_BasicModelLibrary.Initialize();
        m_BasicModelLibrary.InitializeFiles();
}

void Assets::RefreshAssets()
{
        m_BasicModelLibrary.RefreshAssetsFromFilesystem();
}

void Assets::MergeLoadedAssets()
{
        m_BasicModelLibrary.MergeLoadedAssets();
}
}