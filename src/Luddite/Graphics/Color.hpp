#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/Core/Core.hpp"

namespace Luddite
{
class LUDDITE_API Color
{
public:
        Color()
        {
                SetColorRGBA(0xFFFFFFFF);
        }
        Color(uint32_t hex_color)
        {
                SetColorRGBA(hex_color);
        }
        Color(const glm::vec3& color_vec) : rgba(color_vec, 1.f) {}
        Color(const glm::vec4& color_vec) : rgba{color_vec} {}

        inline float GetRed() const {return rgba.r;}
        inline float GetGreen() const {return rgba.g;}
        inline float GetBlue() const {return rgba.b;}
        inline float GetAlpha() const {return rgba.a;}

        inline void SetRed(float r) {rgba.r = r;}
        inline void SetGreen(float g) {rgba.g = g;}
        inline void SetBlue(float b) {rgba.b = b;}
        inline void SetAlpha(float a) {rgba.a = a;}

        inline void SetColorRGB(uint32_t hex_color)
        {
                rgba.r = float((hex_color >> 16) & 0xFF) / 255.f;
                rgba.g = float((hex_color >> 8) & 0xFF) / 255.f;
                rgba.b = float(hex_color & 0xFF) / 255.f;
        }
        inline void SetColorRGBA(uint32_t hex_color)
        {
                rgba.r = float((hex_color >> 24) & 0xFF) / 255.f;
                rgba.g = float((hex_color >> 16) & 0xFF) / 255.f;
                rgba.b = float((hex_color >> 8) & 0xFF) / 255.f;
                rgba.a = float(hex_color & 0xFF) / 255.f;
        }

        inline glm::vec4 GetVec4() const {return rgba;}
        inline glm::vec3 GetVec3() const {return rgba;}
private:
        glm::vec4 rgba;
};
class LUDDITE_API ColorRGB
{
public:
        ColorRGB()
        {
                SetColorRGB(0xFFFFFF);
        }
        ColorRGB(uint32_t hex_color)
        {
                SetColorRGB(hex_color);
        }
        ColorRGB(const glm::vec3& color_vec) : rgb{color_vec} {}

        inline float GetRed() const {return rgb.r;}
        inline float GetGreen() const {return rgb.g;}
        inline float GetBlue() const {return rgb.b;}

        inline void SetRed(float r) {rgb.r = r;}
        inline void SetGreen(float g) {rgb.g = g;}
        inline void SetBlue(float b) {rgb.b = b;}

        inline void SetColorRGB(uint32_t hex_color)
        {
                rgb.r = float((hex_color >> 16) & 0xFF) / 255.f;
                rgb.g = float((hex_color >> 8) & 0xFF) / 255.f;
                rgb.b = float(hex_color & 0xFF) / 255.f;
        }

        inline glm::vec3 GetVec3() const {return rgb;}
private:
        glm::vec3 rgb;
};
}