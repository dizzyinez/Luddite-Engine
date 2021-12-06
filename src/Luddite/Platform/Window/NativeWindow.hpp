#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/Core/Core.hpp"

#include "Luddite/Platform/Window/GLFWWindow.hpp"

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

#ifdef LD_PLATFORM_WINDOWS
#include "Luddite/Platform/Window/WindowsWindow.hpp"
#include "Luddite/Platform/Window/D3D12Window.hpp"
namespace Luddite
{
using NativeD3D12Window = D3D12Window;
}
#endif //LD_PLATFORM_WINDOWS
