cmake_minimum_required (VERSION 3.6)
project(Editor LANGUAGES CXX VERSION 0.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB_RECURSE EditorSOURCE "src/*.c" "src/*.cc" "src/*.cpp" "src/*.h" "src/*.hpp")
add_executable(Editor WIN32 ${EditorSOURCE})
target_compile_definitions(Editor PRIVATE ${PLATFORM_DEFINE} ${CONFIGURATION_DEFINE})
set(LUDDITE_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(Editor PRIVATE ${LUDDITE_INCLUDE})
target_link_libraries(Editor 
Luddite-Engine
)
target_precompile_headers(Editor PRIVATE src/Editor/pch.hpp)

file(COPY ${CMAKE_SOURCE_DIR}/Assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
