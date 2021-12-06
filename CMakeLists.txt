cmake_minimum_required (VERSION 3.6)

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

project(Luddite-Engine LANGUAGES CXX C VERSION 0.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# set(CMAKE_CXX_STANDARD_REQUIRED True)
if(LINUX)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif()

# if(WIN32)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS")
# endif()
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/assimp)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/glfw)

set(DILIGENT_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(DILIGENT_BUILD_FX OFF CACHE BOOL "" FORCE)
set(DILIGENT_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(DILIGENT_NO_FORMAT_VALIDATION ON CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/DiligentCore)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/DiligentTools)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/spdlog)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/yaml-cpp)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/flecs)

set(BUILD_CPU_DEMOS OFF CACHE BOOL "" FORCE)
set(BUILD_ENET OFF CACHE BOOL "" FORCE)
set(BUILD_CLSOCKET OFF CACHE BOOL "" FORCE)
set(BUILD_OPENGL3_DEMOS OFF CACHE BOOL "" FORCE)
set(BUILD_BULLET2_DEMOS OFF CACHE BOOL "" FORCE)
set(BUILD_EXTRAS OFF CACHE BOOL "" FORCE)
set(BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/bullet3)

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
    set (CONFIGURATION_DEFINE LD_DEBUG)
    if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        set(COMPILE_OPTIONS ${COMPILE_OPTIONS} /DEBUG)
        set(LINK_OPTIONS ${LINK_OPTIONS} "/OPT:REF /OPT:ICF")
    endif()
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    message("Release Configuration")
    set (CONFIGURATION_DEFINE LD_RELEASE)
endif()

file(GLOB_RECURSE ENGINE_SOURCE "src/*.c" "src/*.cc" "src/*.cpp")
file(GLOB_RECURSE ENGINE_HEADERS PARENT_SCOPE "src/*.h" "src/*.hpp")

file(GLOB IMGUIZMO_SOURCE "ThirdParty/ImGuizmo/*.h" "ThirdParty/ImGuizmo/*.cpp")


add_library(Luddite-Engine SHARED ${ENGINE_SOURCE} ${ENGINE_HEADERS} ${IMGUIZMO_SOURCE})
target_compile_options(Luddite-Engine PRIVATE -DUNICODE -DENGINE_DLL=0 ${COMPILE_OPTIONS})
if(LINK_OPTIONS)
set_target_properties(Luddite-Engine PROPERTIES LINK_FLAGS ${LINK_OPTIONS})
endif()
# target_link_options(Luddite-Engine PRIVATE ${LINK_OPTIONS})
target_compile_definitions(Luddite-Engine PRIVATE ${ENGINE_COMPILE_DEFINES} ${PLATFORM_DEFINE} ${CONFIGURATION_DEFINE})

set(LUDDITE_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/src "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/DiligentCore"
"${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/DiligentTools"
"${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/spdlog/include"
"${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/flecs/include"
"${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/glm"
"${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/OBJ-Loader/Source"
"${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/ImGuizmo/"
"${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/libxkbcommon/include"
"${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/bullet3/src"
"${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/yaml-cpp/include"
"${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/glfw/include"
)
target_include_directories(Luddite-Engine PUBLIC ${LUDDITE_INCLUDE} 
#"ThirdParty/imgui"
)

target_precompile_headers(Luddite-Engine PRIVATE src/Luddite/Core/pch.hpp src/Luddite/Graphics/DiligentInclude.hpp)

#set(DILIGENT_INSTALL_TOOLS true)
set(DILIGENT_NO_FORMAT_VALIDATION ON CACHE BOOL "" FORCE)

Set(DILIGENT_LIBS
    Diligent-GraphicsTools
    Diligent-TextureLoader
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
    Set(PLATFORM_LIBS
    ${PLATFORM_LIBS}
    xcb
    GL
    GLU
    xkbcommon-x11
    )
endif()
if (WIN32)
    Set(PLATFORM_LIBS
    ${PLATFORM_LIBS}
    Diligent-ShaderTools
    )
endif()
target_link_libraries(Luddite-Engine ${DILIGENT_LIBS} ${PLATFORM_LIBS} 
glfw
spdlog 
yaml-cpp
Bullet3Dynamics Bullet3Collision Bullet3Geometry BulletInverseDynamics BulletSoftBody BulletCollision BulletDynamics LinearMath Bullet3Common
assimp
flecs
)
target_link_directories(Luddite-Engine PUBLIC
"${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/libxkbcommon/bin/"
)
# copy_required_dlls(Luddite-Engine)
# add_subdirectory(Apps)

set(ENGINE_PLATFORM_DEFINE ${PLATFORM_DEFINE} PARENT_SCOPE)
set(ENGINE_CONFIGURATION_DEFINE ${CONFIGURATION_DEFINE} PARENT_SCOPE)
set(ENGINE_INCLUDE ${LUDDITE_INCLUDE} PARENT_SCOPE)
