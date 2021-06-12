#pragma once

//API defines
#ifdef LD_PLATFORM_LINUX
    #define LUDDITE_API
#endif

#ifdef LD_PLATFORM_MACOS
    #define LUDDITE_API
#endif

#ifdef LD_PLATFORM_WINDOWS
    #ifdef LD_BUILD_LIB
        #define LUDDITE_API __declspec(dllexport)
    #else
        #define LUDDITE_API __declspec(dllimport)
    #endif
#include "Luddite/Platform/Compatibility/Windows.hpp"
#endif

//Platform defines
#ifdef LD_PLATFORM_WINDOWS
    #define LD_PLATFORM "LD_PLATFORM_WINDOWS"
    #define LD_PLATFORM_DESKTOP
#endif

#ifdef LD_PLATFORM_LINUX
    #define LD_PLATFORM "LD_PLATFORM_LINUX"
    #define LD_PLATFORM_DESKTOP
#endif

#ifdef LD_PLATFORM_MACOS
    #define LD_PLATFORM "LD_PLATFORM_MACOS"
    #define LD_PLATFORM_DESKTOP
#endif

#ifdef LD_PLATFORM_IOS
    #define LD_PLATFORM "LD_PLATFORM_IOS"
    #define LD_PLATFORM_MOBILE
#endif

#ifdef LD_PLATFORM_ANDRIOD
    #define LD_PLATFORM "LD_PLATFORM_ANDRIOD"
    #define LD_PLATFORM_MOBILE
#endif

//Configuration
#ifdef LD_DEBUG
    #define LD_CONFIGURATION "LD_DEBUG"
    #define LD_ENABLE_IMGUI
#else
    #define LD_CONFIURATION "LD_RELEASE"
#endif