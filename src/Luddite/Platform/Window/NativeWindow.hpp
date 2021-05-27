#pragma once
#include "Luddite/pch.hpp"
#include "Luddite/Core.hpp"

#ifdef LD_PLATFORM_LINUX
#include "Luddite/Platform/Window/XCBWindow.hpp"
#include "Luddite/Platform/Window/GLXWindow.hpp"
namespace Luddite
{
using NativeVulkanWindow = XCBWindow;
using NativeOpenGLWindow = GLXWindow;
// using NativeOpenGLWindow = XWindow;
}
#endif // LD_PLATFORM_LINUX