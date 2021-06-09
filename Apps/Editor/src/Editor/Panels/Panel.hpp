#pragma once
#include "Editor/pch.hpp"

struct Panel
{
        public:
        void Draw(Luddite::World& world)
        {
                if (ShowWindow)
                {
                        if (ImGui::Begin(m_Name.c_str(), &ShowWindow, m_WindowFlags))
                                DrawImplementation(world);
                        ImGui::End();
                }
        }


        protected:
        std::string m_Name = "";
        bool ShowWindow = true;
        virtual void DrawImplementation(Luddite::World& world) = 0;
        ImGuiWindowFlags m_WindowFlags = 0;
};