#pragma once
#include "Luddite/Core/pch.hpp"

namespace Luddite
{
struct ProfilingResult
{
        std::string name;
        long long start;
        long long duration;
        std::thread::id tid;
};

class Profiler
{
        private:
        static bool m_SessionRunning;
        static std::ofstream m_OutputStream;
        static uint32_t m_nProfiles;
        public:
        static void BeginSession(const std::string& name, const std::filesystem::path& outputdir = L"Profiling/");
        static void EndSession();
        static void WriteProfile(const ProfilingResult& result);
        static inline bool IsRunning() {return m_SessionRunning;}
        static inline std::mutex m_Mutex;
};

class ProfileTimer
{
        private:
        std::chrono::time_point<std::chrono::high_resolution_clock> timer_start;
        std::string m_Name;
        bool running = true;
        public:
        ProfileTimer(const std::string& name)
                : m_Name(name)
        {
                timer_start = std::chrono::high_resolution_clock::now();
        }

        ~ProfileTimer()
        {
                StopTimer();
        }

        void StopTimer()
        {
                if (!running)
                        return;
                running = false;

                auto timer_end = std::chrono::high_resolution_clock::now();
                auto start = std::chrono::time_point_cast<std::chrono::microseconds>(timer_start).time_since_epoch().count();
                auto end = std::chrono::time_point_cast<std::chrono::microseconds>(timer_end).time_since_epoch().count();
                // auto duration = end - start;
                std::thread::id thread_id = std::this_thread::get_id();
                Profiler::WriteProfile({m_Name, start, end - start, thread_id});
        }
};
#define ENABLE_PROFILING 1
#if ENABLE_PROFILING
#define LD_PROFILE_SCOPE(name) Luddite::ProfileTimer timer ## __LINE__(name)
#else
#define LD_PROFILE_SCOPE(name) do{}while {}
#endif

#ifdef LD_PLATFORM_WINDOWS
#define LD_PROFILE_FUNCTION() LD_PROFILE_SCOPE(__FUNCSIG__)
#else
#define LD_PROFILE_FUNCTION() LD_PROFILE_SCOPE(__PRETTY_FUNCTION__)
#endif
}