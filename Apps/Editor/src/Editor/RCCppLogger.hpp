#pragma once
#include "Luddite/Core/pch.hpp"
#include "ICompilerLogger.h"

// const size_t LOGSYSTEM_MAX_BUFFER = 4096;

// class RCCppLogger : public ICompilerLogger
// {
//         public:
//         RCCppLogger() {}
//         ~RCCppLogger() override {}
//         virtual void LogError(const char * format, ...) override;
//         virtual void LogWarning(const char * format, ...) override;
//         virtual void LogInfo(const char * format, ...) override;

//         protected:
//         void LogInternal(const char * format, va_list args);
//         char m_buff[LOGSYSTEM_MAX_BUFFER];
// };
const size_t LOGSYSTEM_MAX_BUFFER = 4096;

class RCCppLogger : public ICompilerLogger
{
        public:
        virtual void LogError(const char * format, ...);
        virtual void LogWarning(const char * format, ...);
        virtual void LogInfo(const char * format, ...);

        protected:
        void LogInternal(const char * format, va_list args);
        char m_buff[LOGSYSTEM_MAX_BUFFER];
};
