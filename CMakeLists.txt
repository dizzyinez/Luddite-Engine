cmake_minimum_required (VERSION 3.6)

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

project(Luddite-Engine LANGUAGES CXX C VERSION 0.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# set(CMAKE_CXX_STANDARD_REQUIRED True)
if(LINUX)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif()

# if(WIN32)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS")
# endif()

add_subdirectory(ThirdParty/DiligentCore)
add_subdirectory(ThirdParty/DiligentFX)
add_subdirectory(ThirdParty/DiligentTools)
add_subdirectory(ThirdParty/spdlog)
#add_subdirectory(ThirdParty/imgui)

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
    message("Debug Configuration")
    set (CONFIGURATION_DEFINE LD_DEBUG 
    _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING
    _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
    _ENFORCE_MATCHING_ALLOCATORS=0
    )
    if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        set(COMPILE_OPTIONS ${COMPILE_OPTIONS} /DEBUG)
        set(LINK_OPTIONS ${LINK_OPTIONS} "/OPT:REF /OPT:ICF")
    endif()
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    message("Release Configuration")
    set (CONFIGURATION_DEFINE LD_RELEASE 
    _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING
    _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
    _ENFORCE_MATCHING_ALLOCATORS=0
    )
endif()

file(GLOB_RECURSE ENGINE_SOURCE "src/*.c" "src/*.cc" "src/*.cpp")
file(GLOB_RECURSE ENGINE_HEADERS "src/*.h" "src/*.hpp")

add_library(Luddite-Engine STATIC ${ENGINE_SOURCE} ${ENGINE_HEADERS})
target_compile_options(Luddite-Engine PRIVATE -DUNICODE -DENGINE_DLL=0 ${COMPILE_OPTIONS})
if(LINK_OPTIONS)
set_target_properties(Luddite-Engine PROPERTIES LINK_FLAGS ${LINK_OPTIONS})
endif()
# target_link_options(Luddite-Engine PRIVATE ${LINK_OPTIONS})
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
#"ThirdParty/imgui"
)

target_precompile_headers(Luddite-Engine PRIVATE src/Luddite/Core/pch.hpp src/Luddite/Graphics/DiligentInclude.hpp)

set(DILIGENT_INSTALL_TOOLS true)

Set(DILIGENT_LIBS
    Diligent-GraphicsTools
    Diligent-TextureLoader
    DiligentFX
    Diligent-Imgui
)
if(D3D11_SUPPORTED)
Set(DILIGENT_LIBS ${DILIGENT_LIBS} Diligent-GraphicsEngineD3D11-static)
endif()
if(D3D12_SUPPORTED)
Set(DILIGENT_LIBS ${DILIGENT_LIBS} Diligent-GraphicsEngineD3D12-static)
endif()
if(GL_SUPPORTED)
Set(DILIGENT_LIBS ${DILIGENT_LIBS} Diligent-GraphicsEngineOpenGL-static)
endif()
if(VULKAN_SUPPORTED)
Set(DILIGENT_LIBS ${DILIGENT_LIBS} Diligent-GraphicsEngineVk-static)
endif()

if (LINUX)
    Set(DILIGENT_LIBS
    ${DILIGENT_LIBS}
    xcb
    GL
    GLU
    )
endif()
if (WIN32)
    Set(DILIGENT_LIBS
    ${DILIGENT_LIBS}
    Diligent-ShaderTools
    )
endif()
target_link_libraries(Luddite-Engine ${DILIGENT_LIBS} spdlog)
# copy_required_dlls(Luddite-Engine)
add_subdirectory(Apps)
