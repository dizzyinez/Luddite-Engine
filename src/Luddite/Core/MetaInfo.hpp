#pragma once
#include "Luddite/Core/Logging.hpp"
#include "Luddite/Core/pch.hpp"
#include "Luddite/Core/map.h"
#include "glm/fwd.hpp"
#include "imgui.h"

template<typename T>
struct MetaTypeInfo
{
        std::string_view name;
        static constexpr std::size_t size() {return sizeof(T);}
        //static std::string to_string(const T& t) {return std::to_string(t);}
        void IMGuiElement(T& t) const {};
};

#define LD_GUI_ELEMENT_DRAGNUM 0
#define LD_GUI_ELEMENT_SLIDER 1
#define LD_GUI_ELEMENT_SLIDERANGLE 2

template<>
struct MetaTypeInfo<float>
{
        const std::string name;
        float speed = 1.0;
        float min = FLT_MIN;
        float max = FLT_MAX;
        int gui_elem = LD_GUI_ELEMENT_DRAGNUM;
        std::string format{"%.3f"};
        int flags = 0;

        static constexpr std::size_t size() {return sizeof(float);}
        static std::string to_string(const float& f) {return std::to_string(f);}
        void IMGuiElement(float & f) const
        {
                switch (gui_elem)
                {
                case LD_GUI_ELEMENT_DRAGNUM:
                        ImGui::DragFloat(name.c_str(), &f, speed, min, max, format.c_str(), flags);
                        break;

                case LD_GUI_ELEMENT_SLIDER:
                        ImGui::SliderFloat(name.c_str(), &f, min, max, format.c_str(), flags);
                        break;

                case LD_GUI_ELEMENT_SLIDERANGLE:
                        ImGui::SliderAngle(name.c_str(), &f, min, max, format.c_str(), flags);
                        break;
                }
                ;
        };
};
#define REM(x) x
#define TYPEINFO(T, IMFUNC) \
        template<> \
        struct MetaTypeInfo<T> { \
                const std::string name; \
                static constexpr std::size_t size() {return sizeof(T);} \
                static std::string to_string(const T& t) {return std::to_string(t);} \
                void IMGuiElement(T & t) const {REM IMFUNC}; \
        };
#define GLMTYPEINFO(T, IMFUNC) \
        template<> \
        struct MetaTypeInfo<T> { \
                const std::string name; \
                static constexpr std::size_t size() {return sizeof(T);} \
                static std::string to_string(const T& t) {return glm::to_string(t);} \
                void IMGuiElement(T & t) const {REM IMFUNC}; \
        };
GLMTYPEINFO(glm::vec2, (ImGui::DragFloat2(name.c_str(), glm::value_ptr(t)); ))
GLMTYPEINFO(glm::vec3, (ImGui::DragFloat3(name.c_str(), glm::value_ptr(t)); ))
GLMTYPEINFO(glm::vec4, (ImGui::DragFloat4(name.c_str(), glm::value_ptr(t)); ))
#undef REM
