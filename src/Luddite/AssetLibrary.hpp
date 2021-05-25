#pragma once
#include "Luddite/pch.hpp"

namespace Luddite
{
template <typename T, typename ID = std::string>
struct AssetLibrary
{
private:
        std::unordered_map<ID, T> assetmap;
};
}