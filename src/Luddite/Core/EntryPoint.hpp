#pragma once
#ifdef LD_ENTRYPOINT

#include "Luddite/Core/Core.hpp"
#include "Luddite/Core/Application.hpp"
#include "Luddite/Core/Logging.hpp"

#ifdef LD_PLATFORM_WINDOWS
int WINAPI WinMain(HINSTANCE instance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int cmdShow)
{
        RedirectIOToConsole();
        Luddite::WindowsProperties::cmdShow = cmdShow;
        Luddite::WindowsProperties::instance = instance;
        Luddite::Logger::Init();
        LD_LOG_INFO("Logger Initialized!");
        Luddite::Application* app = Luddite::CreateApplication();
        if (!app)
        {
                LD_LOG_ERROR("Luddite::CreateApplication returned nullptr!");
        }
        app->Run();
        LD_LOG_INFO("Exiting!");
        delete app;
}
#endif //LD_PLATFORM_WINDOWS

#ifdef LD_PLATFORM_LINUX
int main(int argc, char** argv)
{
        Luddite::Logger::Init();
        RegisterECSTypeIds();
        LD_LOG_INFO("Logger Initialized!");
        auto app = Luddite::CreateApplication();
        app->Run();
        delete app;
}
#endif //LD_PLATFORM_LINUX
#endif //LD_ENTRYPOINT