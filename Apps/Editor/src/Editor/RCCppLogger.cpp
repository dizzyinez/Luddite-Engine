#include "Luddite/Core/RCCppLogger.hpp"

#ifdef LD_PLATFORM_WINDOWS
    #include "Windows.h"
        #pragma warning( disable : 4996 4800 )
#endif //LD_PLATFORM_WINDOWS

// // RCCppLogger::RCCppLogger() {}
// // RCCppLogger::~RCCppLogger() {}

// void RCCppLogger::LogError(const char * format, ...)
// {
//         va_list args;
//         va_start(args, format);
//         LogInternal(format, args);
// }

// void RCCppLogger::LogWarning(const char * format, ...)
// {
//         va_list args;
//         va_start(args, format);
//         LogInternal(format, args);
// }

// void RCCppLogger::LogInfo(const char * format, ...)
// {
//         va_list args;
//         va_start(args, format);
//         LogInternal(format, args);
// }
// void RCCppLogger::LogInternal(const char * format, va_list args)
// {
//         vsnprintf(m_buff, LOGSYSTEM_MAX_BUFFER - 1, format, args);
//         // Make sure there's a limit to the amount of rubbish we can output
//         m_buff[LOGSYSTEM_MAX_BUFFER - 1] = '\0';

//         std::cout << m_buff;
// #ifdef LD_PLATFORM_WINDOWS
//         OutputDebugStringA(m_buff);
// #endif //LD_PLATFORM_WINDOWS
// }


void RCCppLogger::LogError(const char * format, ...)
{
        va_list args;
        va_start(args, format);
        LogInternal(format, args);
}

void RCCppLogger::LogWarning(const char * format, ...)
{
        va_list args;
        va_start(args, format);
        LogInternal(format, args);
}

void RCCppLogger::LogInfo(const char * format, ...)
{
        va_list args;
        va_start(args, format);
        LogInternal(format, args);
}
void RCCppLogger::LogInternal(const char * format, va_list args)
{
        vsnprintf(m_buff, LOGSYSTEM_MAX_BUFFER - 1, format, args);
        // Make sure there's a limit to the amount of rubbish we can output
        m_buff[LOGSYSTEM_MAX_BUFFER - 1] = '\0';

        std::cout << m_buff;
#ifdef _WIN32
        OutputDebugStringA(m_buff);
#endif
}