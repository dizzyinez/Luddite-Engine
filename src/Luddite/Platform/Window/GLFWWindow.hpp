#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/Platform/Window/Window.hpp"

struct GLFWwindow;
namespace Luddite
{
class LUDDITE_API GLFWWindow : public Window
{
        public:
        GLFWWindow(const std::string& title = "Luddite Engine Application", int width = 1024, int height = 768, int min_width = 320, int min_height = 240);
        ~GLFWWindow() override;
        void SetTitle(const std::string& title) override;
        void PollEvents() override;
        private:
        GLFWwindow* m_Window;
        static void GLFW_ResizeCallback(GLFWwindow* wnd, int w, int h);
        static void GLFW_KeyCallback(GLFWwindow* wnd, int key, int scancode, int action, int mods);
        static void GLFW_MouseButtonCallback(GLFWwindow* wnd, int button, int action, int mods);
        static void GLFW_CursorPosCallback(GLFWwindow* wnd, double xpos, double ypos);
        static void GLFW_CharCallback(GLFWwindow* wnd, unsigned int c);
        static void GLFW_ScrollCallback(GLFWwindow* wnd, double xoffset, double yoffset);
        static const char* ImGui_ImplGlfw_GetClipboardText(void* user_data);
        static void ImGui_ImplGlfw_SetClipboardText(void* user_data, const char* text);
};
}
