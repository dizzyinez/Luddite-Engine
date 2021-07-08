#include "Luddite/Core/pch.hpp"
#include "Luddite/Core/AssetLibrary.hpp"
#include "Luddite/Core/AssetTypes/Model.hpp"

namespace Luddite
{
struct LUDDITE_API Assets
{
        private:
        static BasicModelLibrary m_BasicModelLibrary;

        public:
        static void Initialize();
        static void MergeLoadedAssets();
        static void RefreshAssets();
        static BasicModelLibrary& GetBasicModelLibrary() {return m_BasicModelLibrary;}
};
}