# Install script for directory: /home/evan/Documents/Dev/Luddite-Engine/ThirdParty/DiligentCore/ThirdParty

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/bin/x86_64-linux-gnu-objdump")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/ThirdParty/DiligentCore/Debug" TYPE STATIC_LIBRARY FILES "/home/evan/Documents/Dev/Luddite-Engine/build/ThirdParty/DiligentCore/ThirdParty/glew/libglew-static.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Licenses/ThirdParty/ThirdParty/DiligentCore" TYPE FILE RENAME "GLEW-License.txt" FILES "/home/evan/Documents/Dev/Luddite-Engine/ThirdParty/DiligentCore/ThirdParty/glew/LICENSE.txt")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Licenses/ThirdParty/ThirdParty/DiligentCore" TYPE FILE RENAME "SPIRV-Headers-License.txt" FILES "/home/evan/Documents/Dev/Luddite-Engine/ThirdParty/DiligentCore/ThirdParty/SPIRV-Headers/LICENSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Licenses/ThirdParty/ThirdParty/DiligentCore" TYPE FILE RENAME "Vulkan-Headers-License.txt" FILES "/home/evan/Documents/Dev/Luddite-Engine/ThirdParty/DiligentCore/ThirdParty/Vulkan-Headers/LICENSE.txt")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Licenses/ThirdParty/ThirdParty/DiligentCore" TYPE FILE RENAME "SPIRV-Tools-License.txt" FILES "/home/evan/Documents/Dev/Luddite-Engine/ThirdParty/DiligentCore/ThirdParty/SPIRV-Tools/LICENSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Licenses/ThirdParty/ThirdParty/DiligentCore" TYPE FILE RENAME "GLSLang-License.txt" FILES "/home/evan/Documents/Dev/Luddite-Engine/ThirdParty/DiligentCore/ThirdParty/glslang/LICENSE.txt")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Licenses/ThirdParty/ThirdParty/DiligentCore" TYPE FILE RENAME "SPIRV-Cross-License.txt" FILES "/home/evan/Documents/Dev/Luddite-Engine/ThirdParty/DiligentCore/ThirdParty/SPIRV-Cross/LICENSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/ThirdParty/DiligentCore/Debug" TYPE STATIC_LIBRARY FILES "/home/evan/Documents/Dev/Luddite-Engine/build/ThirdParty/DiligentCore/ThirdParty/SPIRV-Tools/source/libSPIRV-Tools.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/ThirdParty/DiligentCore/Debug" TYPE STATIC_LIBRARY FILES "/home/evan/Documents/Dev/Luddite-Engine/build/ThirdParty/DiligentCore/ThirdParty/SPIRV-Tools/source/opt/libSPIRV-Tools-opt.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/ThirdParty/DiligentCore/Debug" TYPE STATIC_LIBRARY FILES "/home/evan/Documents/Dev/Luddite-Engine/build/ThirdParty/DiligentCore/ThirdParty/glslang/glslang/libGenericCodeGen.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/ThirdParty/DiligentCore/Debug" TYPE STATIC_LIBRARY FILES "/home/evan/Documents/Dev/Luddite-Engine/build/ThirdParty/DiligentCore/ThirdParty/glslang/glslang/libglslang.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/ThirdParty/DiligentCore/Debug" TYPE STATIC_LIBRARY FILES "/home/evan/Documents/Dev/Luddite-Engine/build/ThirdParty/DiligentCore/ThirdParty/glslang/OGLCompilersDLL/libOGLCompiler.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/ThirdParty/DiligentCore/Debug" TYPE STATIC_LIBRARY FILES "/home/evan/Documents/Dev/Luddite-Engine/build/ThirdParty/DiligentCore/ThirdParty/glslang/glslang/OSDependent/Unix/libOSDependent.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/ThirdParty/DiligentCore/Debug" TYPE STATIC_LIBRARY FILES "/home/evan/Documents/Dev/Luddite-Engine/build/ThirdParty/DiligentCore/ThirdParty/glslang/SPIRV/libSPIRV.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/ThirdParty/DiligentCore/Debug" TYPE STATIC_LIBRARY FILES "/home/evan/Documents/Dev/Luddite-Engine/build/ThirdParty/DiligentCore/ThirdParty/glslang/hlsl/libHLSL.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/ThirdParty/DiligentCore/Debug" TYPE STATIC_LIBRARY FILES "/home/evan/Documents/Dev/Luddite-Engine/build/ThirdParty/DiligentCore/ThirdParty/glslang/glslang/libMachineIndependent.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/ThirdParty/DiligentCore/Debug" TYPE STATIC_LIBRARY FILES "/home/evan/Documents/Dev/Luddite-Engine/build/ThirdParty/DiligentCore/ThirdParty/SPIRV-Cross/libspirv-cross-core.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Licenses/ThirdParty/ThirdParty/DiligentCore" TYPE FILE RENAME "googletest-License.txt" FILES "/home/evan/Documents/Dev/Luddite-Engine/ThirdParty/DiligentCore/ThirdParty/googletest/LICENSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Licenses/ThirdParty/ThirdParty/DiligentCore" TYPE FILE FILES "/home/evan/Documents/Dev/Luddite-Engine/ThirdParty/DiligentCore/ThirdParty/stb/stb_image_write_license.txt")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Licenses/ThirdParty/ThirdParty/DiligentCore" TYPE FILE RENAME "Volk-License.md" FILES "/home/evan/Documents/Dev/Luddite-Engine/ThirdParty/DiligentCore/ThirdParty/volk/LICENSE.md")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Licenses/ThirdParty/ThirdParty/DiligentCore" TYPE FILE RENAME "DXC-License.txt" FILES "/home/evan/Documents/Dev/Luddite-Engine/ThirdParty/DiligentCore/ThirdParty/DirectXShaderCompiler/LICENSE.TXT")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Licenses/ThirdParty/ThirdParty/DiligentCore" TYPE FILE RENAME "DXC-ThirdPartyNotices.txt" FILES "/home/evan/Documents/Dev/Luddite-Engine/ThirdParty/DiligentCore/ThirdParty/DirectXShaderCompiler/ThirdPartyNotices.txt")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/evan/Documents/Dev/Luddite-Engine/build/ThirdParty/DiligentCore/ThirdParty/glew/cmake_install.cmake")
  include("/home/evan/Documents/Dev/Luddite-Engine/build/ThirdParty/DiligentCore/ThirdParty/SPIRV-Tools/cmake_install.cmake")
  include("/home/evan/Documents/Dev/Luddite-Engine/build/ThirdParty/DiligentCore/ThirdParty/glslang/cmake_install.cmake")
  include("/home/evan/Documents/Dev/Luddite-Engine/build/ThirdParty/DiligentCore/ThirdParty/SPIRV-Cross/cmake_install.cmake")

endif()

