#include "Application.hpp"

namespace Luddite
{
Application::Application()
{}
Application::~Application()
{}
void Application::Run()
{
        for (auto window : m_windows)
                window->GetRenderer().Initialize();
        while (true)
        {
                for (auto window : m_windows)
                {
                        window->GetRenderer().Draw();
                        window->GetRenderer().Present();
                }
        }
}
}
