#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/Core/Core.hpp"

#ifdef LD_PLATFORM_LINUX
#include "Luddite/Platform/Window/GLFWWindow.hpp"
namespace Luddite
{
using NativeWindow = GLFWWindow;
}
#endif // LD_PLATFORM_LINUX

#ifdef LD_PLATFORM_WINDOWS
#include "Luddite/Platform/Window/GLFWWindow.hpp"
namespace Luddite
{
using NativeWindow = GLFWWindow;
}
#endif //LD_PLATFORM_WINDOWS
