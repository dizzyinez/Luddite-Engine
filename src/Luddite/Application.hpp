#pragma once
#include "Luddite/pch.hpp"
#include "Luddite/Core.hpp"
#include "Luddite/Platform/Window/Window.hpp"

namespace Luddite
{
class LUDDITE_API Application
{
public:
        Application();
        virtual ~Application();
        void Run();
protected:
        std::vector<std::shared_ptr<Window> > m_windows;
};

Application* CreateApplication();
}
