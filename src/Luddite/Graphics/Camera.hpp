#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/Core/Core.hpp"
#include "Luddite/Core/MetaInfo.hpp"

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
        } Projection = ProjectionType::PERSPECTIVE;
        float FOV = glm::radians(90.f);
        float OrthoScale = 100.f;
        float ClipNear = 0.1f;
        float ClipFar = 1000.f;
};
}

template <>
struct MetaTypeInfo<Luddite::Camera::ProjectionType>
{
        std::string_view name;
        static constexpr std::size_t size() {return sizeof(Luddite::Camera::ProjectionType);}
        void IMGuiElement(Luddite::Camera::ProjectionType& p) const
        {
                const char* items[] = {"Perspective", "OrthoGraphic"};
                int curr = (int)p;
                //switch (p)
                //{
                //case Luddite::Camera::ProjectionType::PERSPECTIVE:
                //        curr = 0;
                //        break;

                //case Luddite::Camera::ProjectionType::ORTHOGRAPHIC:
                //        curr = 1;
                //        break;
                //}
                ImGui::ListBox("Projection Type", &curr, items, IM_ARRAYSIZE(items));
                p = (Luddite::Camera::ProjectionType)curr;
        };
};
