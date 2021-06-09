# Install script for directory: /home/evan/Documents/Dev/Luddite-Engine/ThirdParty/DiligentFX

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/ThirdParty/DiligentFX/Debug" TYPE STATIC_LIBRARY FILES "/home/evan/Documents/Dev/Luddite-Engine/build/ThirdParty/DiligentFX/libDiligentFX.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ThirdParty/DiligentFX/PostProcess/EpipolarLightScattering" TYPE DIRECTORY FILES "/home/evan/Documents/Dev/Luddite-Engine/ThirdParty/DiligentFX/PostProcess/EpipolarLightScattering/interface")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ThirdParty/DiligentFX/Components" TYPE DIRECTORY FILES "/home/evan/Documents/Dev/Luddite-Engine/ThirdParty/DiligentFX/Components/interface")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ThirdParty/DiligentFX/GLTF_PBR_Renderer" TYPE DIRECTORY FILES "/home/evan/Documents/Dev/Luddite-Engine/ThirdParty/DiligentFX/GLTF_PBR_Renderer/interface")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/." TYPE DIRECTORY FILES "/home/evan/Documents/Dev/Luddite-Engine/ThirdParty/DiligentFX/Shaders" FILES_MATCHING REGEX "/public\\/[^/]*\\.[^/]*$" REGEX "/private$" EXCLUDE)
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/evan/Documents/Dev/Luddite-Engine/build/ThirdParty/DiligentFX/Utilities/cmake_install.cmake")
  include("/home/evan/Documents/Dev/Luddite-Engine/build/ThirdParty/DiligentFX/Components/cmake_install.cmake")
  include("/home/evan/Documents/Dev/Luddite-Engine/build/ThirdParty/DiligentFX/PostProcess/cmake_install.cmake")
  include("/home/evan/Documents/Dev/Luddite-Engine/build/ThirdParty/DiligentFX/GLTF_PBR_Renderer/cmake_install.cmake")

endif()

