#include "Luddite/Platform/Window/GLFWWindow.hpp"

#include "GLFW/glfw3.h"

#ifndef D3D11_SUPPORTED
#    define D3D11_SUPPORTED 0
#endif

#ifndef D3D12_SUPPORTED
#    define D3D12_SUPPORTED 0
#endif

#ifndef GL_SUPPORTED
#    define GL_SUPPORTED 0
#endif

#ifndef VULKAN_SUPPORTED
#    define VULKAN_SUPPORTED 0
#endif

#ifndef METAL_SUPPORTED
#    define METAL_SUPPORTED 0
#endif

#ifdef LD_PLATFORM_WINDOWS
#    define GLFW_EXPOSE_NATIVE_WIN32 1
#endif

#ifdef LD_PLATFORM_LINUX
#include "X11/Xlib.h"
#include "Imgui/interface/ImGuiImplLinuxX11.hpp"
#    define GLFW_EXPOSE_NATIVE_X11 1
#endif

#ifdef LD_PLATFORM_MACOS
#    define GLFW_EXPOSE_NATIVE_COCOA 1
#endif

#if D3D11_SUPPORTED
#    include "Graphics/GraphicsEngineD3D11/interface/EngineFactoryD3D11.h"
#endif
#if D3D12_SUPPORTED
#    include "Graphics/GraphicsEngineD3D12/interface/EngineFactoryD3D12.h"
#endif
#if GL_SUPPORTED
#    include "Graphics/GraphicsEngineOpenGL/interface/EngineFactoryOpenGL.h"
#endif
#if VULKAN_SUPPORTED
#    include "Graphics/GraphicsEngineVulkan/interface/EngineFactoryVk.h"
#endif
#if METAL_SUPPORTED
#    include "Graphics/GraphicsEngineMetal/interface/EngineFactoryMtl.h"
#endif

#ifdef LD_PLATFORM_WINDOWS
#    undef GetObject
#    undef CreateWindow
#endif

#include "GLFW/glfw3native.h"

#if PLATFORM_MACOS
extern void* GetNSWindowView(GLFWwindow* wnd);
#endif

namespace Luddite
{
GLFWWindow::GLFWWindow(const std::string& title, int width, int height, int min_width, int min_height)
{
        //Create GLFW Window
        if (glfwInit() != GLFW_TRUE)
                LD_LOG_CRITICAL("Failed To Initialize GLFW");

        int APIHint = GLFW_NO_API;
#ifndef LD_PLATFORM_WINDOWS
        if (false /*TODO: using opengl*/)
                APIHint = GLFW_OPENGL_API;
#endif
        glfwWindowHint(GLFW_CLIENT_API, APIHint);
        if (APIHint == GLFW_OPENGL_API)
        {
                glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        }

        m_Window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
        LD_VERIFY(m_Window, "Failed to create main GLFW Window");
        glfwSetWindowSizeLimits(m_Window, min_width, min_height, GLFW_DONT_CARE, GLFW_DONT_CARE);

        //set callbacks
        glfwSetWindowUserPointer(m_Window, this);
        glfwSetFramebufferSizeCallback(m_Window, GLFW_ResizeCallback);




#if LD_PLATFORM_LINUX
        Diligent::LinuxNativeWindow window;
        window.WindowId = glfwGetX11Window(m_Window);
        window.pDisplay = glfwGetX11Display();

        auto* pFactoryVk = Diligent::GetEngineFactoryVk();
        Renderer::GetEngineFactory() = pFactoryVk;
        Diligent::EngineVkCreateInfo EngineCI;
        pFactoryVk->CreateDeviceAndContextsVk(EngineCI, &Renderer::GetDevice(), &Renderer::GetContext());
        {
                Diligent::SwapChainDesc SCDesc;
                pFactoryVk->CreateSwapChainVk(Renderer::GetDevice(), Renderer::GetContext(), SCDesc, window, &m_pSwapChain);
        }

        auto& SCDesc = m_pSwapChain->GetDesc();
        Renderer::SetDefaultRTVFormat(SCDesc.ColorBufferFormat);
        Renderer::SetDefaultDSVFormat(SCDesc.DepthBufferFormat);
        m_pImGuiImpl.reset(new Diligent::ImGuiImplLinuxX11(
                Renderer::GetDevice(),
                SCDesc.ColorBufferFormat,
                SCDesc.DepthBufferFormat,
                SCDesc.Width,
                SCDesc.Height
                ));

        ImGuiSetup();
        m_pImGuiImpl->CreateDeviceObjects();

#endif
        OnWindowResize(width, height);
}

GLFWWindow::~GLFWWindow()
{}

void GLFWWindow::SetTitle(const std::string &title)
{
}

void GLFWWindow::PollEvents()
{
        glfwPollEvents();
}

void GLFWWindow::GLFW_ResizeCallback(GLFWwindow* wnd, int w, int h)
{
        auto* pSelf = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(wnd));
        pSelf->OnWindowResize(w, h);
        auto& io = ImGui::GetIO();
        io.DisplaySize = ImVec2(w, h);
}
}
