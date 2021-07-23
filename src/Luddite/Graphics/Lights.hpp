#pragma once
#include "Luddite/Core/pch.hpp"

namespace Luddite
{
struct alignas (16) PointLightGPU
{
        glm::vec4 PositionWS = glm::vec4{};
        glm::vec4 PositionVS = glm::vec4{};
        glm::vec3 Color = glm::vec3(1.f);
        float Range = 100.0f;
        float Intensity = 1.0f;
        uint32_t Enabled = 1;
        uint32_t Selected = 0;
        float Padding = 0.f;
};
struct PointLightCPU
{
        glm::vec4 Position;
        glm::vec3 Color = glm::vec3(1.f);
        float Range = 100.f;
        float Intensity = 1.f;
};
struct alignas (16) SpotLightGPU
{
        glm::vec4 PositionWS = glm::vec4{};
        glm::vec4 PositionVS = glm::vec4{};
        glm::vec4 DirectionWS = glm::vec4{};
        glm::vec4 DirectionVS = glm::vec4{};
        glm::vec3 Color = glm::vec3(1.f);
        float HalfAngle = glm::quarter_pi<float>(); //Radians
        float Range = 100.f;
        float Intensity = 1.f;
        uint32_t Enabled = 1;
        uint32_t Selected = 0;
};
struct SpotLightCPU
{
        glm::vec4 Position = glm::vec4{};
        glm::vec4 Direction = glm::vec4{};
        glm::vec3 Color = glm::vec3(1.f);
        float HalfAngle = glm::quarter_pi<float>(); //Radians
        float Range = 100.f;
        float Intensity = 1.f;
};
struct alignas (16) DirectionalLightGPU
{
        glm::vec4 DirectionWS = glm::vec4{};
        glm::vec4 DirectionVS = glm::vec4{};
        glm::vec3 Color = glm::vec3(1.f);
        float Intensity = 1.f;
        uint32_t Enabled = 1;
        uint32_t Selected = 0;
        glm::vec2 Padding;
};
struct DirectionalLightCPU
{
        glm::vec4 Direction = glm::vec4{};
        glm::vec3 Color = glm::vec3(1.f);
        float Intensity = 1.f;
};
}