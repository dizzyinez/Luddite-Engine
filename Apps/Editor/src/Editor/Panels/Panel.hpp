#pragma once
#include "Editor/pch.hpp"

struct Panel
{
        public:
        void Draw()
        {
                if (ShowWindow)
                {
                        if (ImGui::Begin(m_Name.c_str(), &ShowWindow, m_WindowFlags))
                                DrawImplementation();
                        ImGui::End();
                }
        }


        protected:
        std::string m_Name = "";
        bool ShowWindow = true;
        virtual void DrawImplementation() = 0;
        ImGuiWindowFlags m_WindowFlags = 0;
};