#include "Core.hpp"
#include "Application.hpp"
extern Luddite::Application* Luddite::CreateApplication();
#include "EntryPoint.hpp"
#include "Logging.hpp"

#ifdef LD_PLATFORM_WINDOWS
int main(int argc, char** argv)
{
}
#endif

#ifdef LD_PLATFORM_LINUX
int main(int argc, char** argv)
{
        Luddite::Logger::Init();
        LOG_INFO("Logger Initialized!");
        auto app = Luddite::CreateApplication();
        app->Run();
        delete app;
}
#endif