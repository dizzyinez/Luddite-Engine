#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/Core/Core.hpp"
#ifdef LD_PLATFORM_WINDOWS
void LUDDITE_API RedirectIOToConsole();

#include <windows.h>
#undef min
#undef max
namespace Luddite
{
class LUDDITE_API WindowsProperties
{
public:
        static inline HINSTANCE instance;
        static inline int cmdShow = 0;
};
}
#endif //LD_PLATFORM_WINDOWS