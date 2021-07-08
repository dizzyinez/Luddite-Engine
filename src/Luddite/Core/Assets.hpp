#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/Core/AssetLibrary.hpp"
#include "Luddite/Core/AssetTypes/Model.hpp"
#include "Luddite/Core/AssetTypes/Texture.hpp"

namespace Luddite
{
struct LUDDITE_API Assets
{
        private:
        static BasicModelLibrary m_BasicModelLibrary;
        static TextureLibrary m_TextureLibrary;

        public:
        static void Initialize();
        static void MergeLoadedAssets();
        static void RefreshAssets();
        static BasicModelLibrary& GetBasicModelLibrary() {return m_BasicModelLibrary;}
        static TextureLibrary& GetTextureLibrary() {return m_TextureLibrary;}
};
}