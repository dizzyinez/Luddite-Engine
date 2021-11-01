#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/Graphics/Camera.hpp"

struct C_Camera
{
        // C_Camera(const Luddite::Camera& Camera_)
        //         : Camera(Camera_)
        // {}

        Luddite::Camera::ProjectionType Projection = Luddite::Camera::ProjectionType::PERSPECTIVE;
        float FOV = glm::radians(90.f);
        float OrthoScale = 100.f;
        float ClipNear = 0.1f;
        float ClipFar = 1000.f;
};
