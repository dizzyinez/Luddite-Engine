#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/Core/Core.hpp"
#include "Luddite/Core/BasicAllocator.hpp"
#include "Luddite/Graphics/Material.hpp"

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
        Diligent::RefCntAutoPtr<Diligent::IBuffer> m_pVertexBuffer;
        Diligent::RefCntAutoPtr<Diligent::IBuffer> m_pIndexBuffer;
        // MaterialHandle material;
};
typedef std::shared_ptr<BasicMesh> BasicMeshHandle;

struct LUDDITE_API BasicModel
{
        std::string name;
        std::vector<BasicMeshHandle> meshes;
};
typedef std::shared_ptr<BasicModel> BasicModelHandle;

class LUDDITE_API BasicModelLibrary : public BasicAllocator<BasicModel, std::string>
{
public:
        // BasicModelHandle Allocate(const std::string& id) override;
private:
};
}