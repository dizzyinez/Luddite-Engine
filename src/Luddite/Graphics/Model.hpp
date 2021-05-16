#pragma once
#include "Luddite/pch.hpp"
#include "Luddite/Core.hpp"

#include "Luddite/Graphics/MaterialLibrary.hpp"

namespace Luddite
{
struct LUDDITE_API BasicVertex
{
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 UV;
};

struct LUDDITE_API BasicMesh
{
        std::string name;
        std::vector<BasicVertex> vertecies;
        std::vector<uint32_t> indicies;
        Material material;
};

struct LUDDITE_API BasicModel
{
        std::string name;
        std::vector<BasicMesh> meshes;
};
}