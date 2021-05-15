#pragma once
#ifdef LD_PLATFORM_WINDOWS
    #define PLATFORM_WIN32 1
#endif

#ifdef LD_PLATFORM_LINUX
    #define PLATFORM_LINUX 1
#endif

#ifdef LD_PLATFORM_MACOS
    #define PLATFORM_MACOS 1
#endif

#ifdef LD_PLATFORM_ANDROID
    #define PLATFORM_ANDROID 1
#endif

#ifdef LD_PLATFORM_IOS
    #define PLATFORM_IOS 1
#endif