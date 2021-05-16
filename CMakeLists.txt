cmake_minimum_required (VERSION 3.6)

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

project(Luddite-Engine LANGUAGES CXX C VERSION 0.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

add_subdirectory(ThirdParty/DiligentCore)
add_subdirectory(ThirdParty/DiligentFX)
add_subdirectory(ThirdParty/DiligentTools)
add_subdirectory(ThirdParty/spdlog)

set (ENGINE_COMPILE_DEFINES LD_BUILD_LIB)
if (LINUX)
    set(PLATFORM_DEFINE LD_PLATFORM_LINUX)
endif()

if (WIN32)
    set(PLATFORM_DEFINE LD_PLATFORM_WINDOWS)
endif()

if (APPLE)
    set(PLATFORM_DEFINE LD_PLATFORM_MACOS)
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set (CONFIGURATION_DEFINE LD_DEBUG)
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set (CONFIGURATION_DEFINE LD_RELEASE)
endif()

file(GLOB_RECURSE ENGINE_SOURCE "src/*.c" "src/*.cc" "src/*.cpp")
file(GLOB_RECURSE ENGINE_HEADERS "src/*.h" "src/*.hpp")

add_library(Luddite-Engine SHARED ${ENGINE_SOURCE} ${ENGINE_HEADERS})
target_compile_options(Luddite-Engine PRIVATE -DUNICODE -DENGINE_DLL)
target_compile_definitions(Luddite-Engine PRIVATE ${ENGINE_COMPILE_DEFINES} ${PLATFORM_DEFINE} ${CONFIGURATION_DEFINE})

set(LUDDITE_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(Luddite-Engine PUBLIC ${LUDDITE_INCLUDE} 
"ThirdParty/DiligentCore"
"ThirdParty/DiligentFX"
"ThirdParty/DiligentTools"
"ThirdParty/spdlog/include"
"ThirdParty/entt/single_include"
"ThirdParty/glm"
"ThirdParty/OBJ-Loader/Source"
)

target_precompile_headers(Luddite-Engine PRIVATE src/Luddite/pch.hpp)

if (LINUX)
    Set(DILIGENT_LIBS
    Diligent-GraphicsEngineOpenGL-shared
    Diligent-GraphicsEngineVk-shared
    Diligent-GraphicsTools
    Diligent-TextureLoader
    DiligentFX
    xcb
    )
endif()
if (WIN32)
    Set(DILIGENT_LIBS
        Diligent-GraphicsEngineD3D11-shared
        Diligent-GraphicsEngineOpenGL-shared
        Diligent-GraphicsEngineD3D12-shared
        Diligent-GraphicsEngineVk-shared
        Diligent-GraphicsTools
        DiligentTools
        DiligentFX
    )
endif()
target_link_libraries(Luddite-Engine PRIVATE ${DILIGENT_LIBS} spdlog)
#copy_required_dlls(Luddite-Engine)
add_subdirectory(Apps)
