cmake_minimum_required (VERSION 3.6)
project(TestApp LANGUAGES CXX VERSION 0.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB_RECURSE TestAppSOURCE "src/*.c" "src/*.cc" "src/*.cpp" "src/*.h" "src/*.hpp")
add_executable(TestApp ${TestAppSOURCE})
target_compile_definitions(TestApp PRIVATE ${PLATFORM_DEFINE} ${CONFIGURATION_DEFINE})
target_include_directories(TestApp PRIVATE ${LUDDITE_INCLUDE})
target_link_libraries(TestApp
PUBLIC
Luddite-Engine
)

file(COPY ${CMAKE_SOURCE_DIR}/Assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
