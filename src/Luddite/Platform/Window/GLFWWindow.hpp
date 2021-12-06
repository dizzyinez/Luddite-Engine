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
};
}
