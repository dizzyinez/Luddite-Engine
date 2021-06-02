#pragma once
#include "Luddite/Core/pch.hpp"

namespace Luddite
{
template <typename T, typename ID = std::string>
struct AssetLibrary
{
private:
        std::unordered_map<ID, T> assetmap;
};
}