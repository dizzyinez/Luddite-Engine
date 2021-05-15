#include "Luddite/pch.hpp"
#include "Luddite/Core.hpp"
#pragma once

namespace Luddite
{
class LUDDITE_API Color
{
public:
        inline float GetRed() const {return rgba.r;}
        inline float GetGreen() const {return rgba.g;}
        inline float GetBlue() const {return rgba.b;}
        inline float GetAlpha() const {return rgba.a;}

        float SetRed(float r) {rgba.r = r;}
        float SetGreen(float g) {rgba.g = g;}
        float SetBlue(float b) {rgba.b = b;}
        float SetAlpha(float a) {rgba.a = a;}

        inline void SetColor(uint32_t hex_color)
        {
                rgba.
        }
private:
        glm::vec4 rgba;
};
}