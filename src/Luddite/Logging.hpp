#pragma once
#include "Luddite/pch.hpp"
#include "Luddite/Core.hpp"
#include "spdlog/spdlog.h"

namespace Luddite
{
class LUDDITE_API Logger
{
public:
        static void Init();
        static inline std::shared_ptr<spdlog::logger> GetCoreConsole() {return core_console;}
        static inline std::shared_ptr<spdlog::logger> GetAppConsole() {return app_console;}

private:
        static std::shared_ptr<spdlog::logger> core_console;
        static std::shared_ptr<spdlog::logger> app_console;
};
}

#ifdef LD_DEBUG
    #ifdef LD_BUILD_LIB
    #define CONSOLE Luddite::Logger::GetCoreConsole()
    #else
    #define CONSOLE Luddite::Logger::GetAppConsole()
    #endif

    #define LOG_TRACE(...) CONSOLE->trace(__VA_ARGS__)
    #define LOG_INFO(...)  CONSOLE->info(__VA_ARGS__)
    #define LOG_WARN(...)  CONSOLE->warn(__VA_ARGS__)
    #define LOG_ERROR(...) CONSOLE->error(__VA_ARGS__)
    #define LOG_CRITICAL(...) CONSOLE->critical(__VA_ARGS__)
#else
    #define LOG_TRACE(...)
    #define LOG_INFO(...)
    #define LOG_WARN(...)
    #define LOG_ERROR(...)
    #define LOG_CRITICAL(...)
#endif