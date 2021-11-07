#pragma once
#include "Luddite/Core/pch.hpp"

namespace Transform3D
{
struct Translation
{
        glm::vec3 Translation = {0.f, 0.f, 0.f};
        inline glm::mat4 GetMatrix() const {return glm::translate(glm::mat4(1.f), Translation);}
};

struct Rotation
{
        glm::vec3 Rotation = {0.f, 0.f, 0.f};
        inline glm::mat4 GetMatrix() const {return glm::toMat4(glm::quat(Rotation));}
        inline glm::vec3 GetLookDirection() const {return glm::toMat4(glm::quat(Rotation)) * glm::vec4(0.f, 0.f, 1.f, 0.f);}
};

struct Scale
{
        glm::vec3 Scale = {1.f, 1.f, 1.f};
        inline glm::mat4 GetMatrix() const {return glm::scale(glm::mat4(1.f), Scale);}
};

struct Components
{
        Components(flecs::world& w)
        {
                w.module<Components>();
                w.component<Translation>();
                w.component<Rotation>();
                w.component<Scale>();
        }
};

struct Systems
{
        Systems(flecs::world& w)
        {
                w.module<Components>();
        }
};
}
