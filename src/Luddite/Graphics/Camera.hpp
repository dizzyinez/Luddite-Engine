#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/Core/Core.hpp"

namespace Luddite
{
struct LUDDITE_API Camera
{
        glm::vec3 Position;
        glm::vec3 ViewDirection;
        enum class ProjectionType : uint8_t
        {
                PERSPECTIVE,
                ORTHOGRAPHIC
        } ProjectionType = ProjectionType::PERSPECTIVE;
        float FOV = glm::radians(90.f);
        float OrthoScale = 100.f;
        float ClipNear = 0.1f;
        float ClipFar = 1000.f;
};
}