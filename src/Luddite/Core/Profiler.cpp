#include "Luddite/Core/Profiler.hpp"
namespace Luddite
{
bool Profiler::m_SessionRunning = false;
std::ofstream Profiler::m_OutputStream;
uint32_t Profiler::m_nProfiles = 0;
void Profiler::BeginSession(const std::string& name, const std::filesystem::path& outputdir)
{
        std::lock_guard lock{m_Mutex};
        if (m_SessionRunning)
                EndSession();
        m_SessionRunning = true;
        m_nProfiles = 0;

        std::filesystem::path path = outputdir / name;
        path.replace_extension(".json");
        m_OutputStream.open(path);
        LD_LOG_INFO("Starting Profling Session: {}", path.string());

        m_OutputStream << "{\"otherData\": {}, \"traceEvents\":[";
        m_OutputStream.flush();
}
void Profiler::EndSession()
{
        std::lock_guard lock{m_Mutex};
        if (!m_SessionRunning)
                return;
        m_SessionRunning = false;
        m_OutputStream << "]}";
        m_OutputStream.flush();
        m_OutputStream.close();
        LD_LOG_INFO("Ending Profling Session");
}
void Profiler::WriteProfile(const ProfilingResult& result)
{
        std::lock_guard lock{m_Mutex};
        if (!m_SessionRunning)
                return;
        if (m_nProfiles > 0) m_OutputStream << ","; //FUCK JSON ALL MY HOMIES HATE JSON
        m_nProfiles++;
        std::string formatted_name(result.name);
        std::replace(formatted_name.begin(), formatted_name.end(), '"', '\'');
        m_OutputStream << "{";
        m_OutputStream << "\"cat\":\"function\",";
        m_OutputStream << "\"dur\":" << result.duration << ",";
        m_OutputStream << "\"name\":\"" << formatted_name << "\",";
        m_OutputStream << "\"ph\":\"X\",";
        m_OutputStream << "\"pid\":0,";
        m_OutputStream << "\"tid\":" << std::hash<std::thread::id>{}(result.tid) << ",";
        m_OutputStream << "\"ts\":" << result.start;
        m_OutputStream << "}";
        m_OutputStream.flush();
}
}
