#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/Graphics/Color.hpp"

struct C_PointLight
{
        Luddite::ColorRGB Color;
        float Range = 10.f;
        float Intensity = 1.f;
};

struct C_SpotLight
{
        Luddite::ColorRGB Color;
        float Range = 10.f;
        glm::vec3 Direction;
        float Intensity = 1.f;
        float HalfAngle = glm::quarter_pi<float>();
};

struct C_DirectionalLight
{
        Luddite::ColorRGB Color;
        float Intensity = 1.f;
};