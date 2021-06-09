#pragma once
#include "Luddite/Core/pch.hpp"

struct C_Transform3D
{
        glm::vec3 Translation = {0.f, 0.f, 0.f};
        glm::vec3 Rotation = {0.f, 0.f, 0.f};
        glm::vec3 Scale = {1.f, 1.f, 1.f};
        glm::mat4 GetTransformMatrix() const
        {
                return glm::translate(glm::mat4(1.f), Translation)
                       * glm::toMat4(glm::quat(Rotation))
                       * glm::scale(glm::mat4(1.f), Scale);
        }

        glm::vec3 GetLookDirection() const
        {
                return glm::vec4(0.f, 0.f, 1.f, 0.f) * glm::toMat4(glm::quat(Rotation));
        }
};