#pragma once
#ifdef LD_PLATFORM_WINDOWS
    #ifdef LD_BUILD_LIB
        #define LUDDITE_API __declspec(dllexport)
    #else
        #define LUDDITE_API __declspec(dllimport)
    #endif
#endif

#ifdef LD_PLATFORM_LINUX
    #define LUDDITE_API
#endif

#ifdef LD_PLATFORM_MACOS
    #define LUDDITE_API
#endif

//to make intellisense happy
#ifndef LUDDITE_API
    #define LUDDITE_API
#endif