#pragma once
#include "Luddite/pch.hpp"
#include "Luddite/Core.hpp"
#include "spdlog/spdlog.h"

#include "Luddite/Platform/DiligentPlatform.hpp"
#include "Primitives/interface/DebugOutput.h"

namespace Luddite
{
class LUDDITE_API Logger
{
public:
        static void Init();
        static void DiligentLogMessage(Diligent::DEBUG_MESSAGE_SEVERITY Severity,
                                       const Diligent::Char*                 Message,
                                       const Diligent::Char*                 Function,
                                       const Diligent::Char*                 File,
                                       int Line)
        {
#ifdef LD_DEBUG
                switch (Severity)
                {
                case Diligent::DEBUG_MESSAGE_SEVERITY_INFO:
                        diligent_console->info(Message);
                        break;

                case Diligent::DEBUG_MESSAGE_SEVERITY_WARNING:
                        diligent_console->warn(Message);
                        break;

                case Diligent::DEBUG_MESSAGE_SEVERITY_ERROR:
                        diligent_console->error(Message);
                        break;

                case Diligent::DEBUG_MESSAGE_SEVERITY_FATAL_ERROR:
                        diligent_console->critical(Message);
                        break;
                }
#endif
        }
        static inline std::shared_ptr<spdlog::logger> GetCoreConsole() {return core_console;}
        static inline std::shared_ptr<spdlog::logger> GetAppConsole() {return app_console;}

private:
        static inline std::shared_ptr<spdlog::logger> GetDiligentConsole() {return diligent_console;}
        static std::shared_ptr<spdlog::logger> core_console;
        static std::shared_ptr<spdlog::logger> app_console;
        static std::shared_ptr<spdlog::logger> diligent_console;
};
}

#ifdef LD_DEBUG
    #ifdef LD_BUILD_LIB
    #define CONSOLE Luddite::Logger::GetCoreConsole()
    #else
    #define CONSOLE Luddite::Logger::GetAppConsole()
    #endif

    #define LD_LOG_TRACE(...) CONSOLE->trace(__VA_ARGS__)
    #define LD_LOG_INFO(...)  CONSOLE->info(__VA_ARGS__)
    #define LD_LOG_WARN(...)  CONSOLE->warn(__VA_ARGS__)
    #define LD_LOG_ERROR(...) CONSOLE->error(__VA_ARGS__)
    #define LD_LOG_CRITICAL(...) CONSOLE->critical(__VA_ARGS__)
#else
    #define LD_LOG_TRACE(...)
    #define LD_LOG_INFO(...)
    #define LD_LOG_WARN(...)
    #define LD_LOG_ERROR(...)
    #define LD_LOG_CRITICAL(...)
#endif