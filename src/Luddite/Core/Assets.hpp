#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/Core/AssetLibrary.hpp"
#include "Luddite/Core/AssetTypes/Model.hpp"
#include "Luddite/Core/AssetTypes/Texture.hpp"
#include "Luddite/Core/AssetTypes/Shader.hpp"
#include "Luddite/Core/AssetTypes/Material.hpp"

namespace Luddite
{
struct LUDDITE_API Assets
{
        private:
        static ModelLibrary m_ModelLibrary;
        static TextureLibrary m_TextureLibrary;
        static ShaderLibrary m_ShaderLibrary;
        static MaterialLibrary m_MaterialLibrary;

        public:
        static void Initialize();
        static void MergeLoadedAssets();
        static void RefreshAssets();
        static ModelLibrary& GetBasicModelLibrary() {return m_ModelLibrary;}
        static TextureLibrary& GetTextureLibrary() {return m_TextureLibrary;}
        static ShaderLibrary& GetShaderLibrary() {return m_ShaderLibrary;}
        static MaterialLibrary& GetMaterialLibrary() {return m_MaterialLibrary;}
};
}