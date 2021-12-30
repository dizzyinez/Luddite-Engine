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
        glfwSetKeyCallback(m_Window, GLFW_KeyCallback);
        glfwSetMouseButtonCallback(m_Window, GLFW_MouseButtonCallback);
        glfwSetCursorPosCallback(m_Window, GLFW_CursorPosCallback);
        glfwSetCharCallback(m_Window, GLFW_CharCallback);
        glfwSetScrollCallback(m_Window, GLFW_ScrollCallback);
        glfwSetWindowCloseCallback(m_Window, GLFW_WindowCloseCallback);




#if LD_PLATFORM_LINUX
        Diligent::LinuxNativeWindow window;
        window.WindowId = glfwGetX11Window(m_Window);
        window.pDisplay = glfwGetX11Display();

        auto* pFactoryVk = Diligent::GetEngineFactoryVk();
        Renderer::GetEngineFactory() = pFactoryVk;
        Diligent::EngineVkCreateInfo EngineCI;
        EngineCI.DynamicHeapSize = 16 << 20;
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

        auto& io = ImGui::GetIO();
        io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
        io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
        io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
        io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
        io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
        io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
        io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
        io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
        io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
        io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
        io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
        io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
        io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
        io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
        io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
        io.KeyMap[ImGuiKey_KeyPadEnter] = GLFW_KEY_KP_ENTER;
        io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
        io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
        io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
        io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
        io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
        io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

        io.SetClipboardTextFn = ImGui_ImplGlfw_SetClipboardText;
        io.GetClipboardTextFn = ImGui_ImplGlfw_GetClipboardText;
        io.ClipboardUserData = m_Window;

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
        auto& io = ImGui::GetIO();
        pSelf->OnWindowResize(w, h);
        io.DisplaySize = ImVec2(w, h);
}
void GLFWWindow::GLFW_KeyCallback(GLFWwindow* wnd, int key, int scancode, int action, int mods)
{
        auto* pSelf = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(wnd));
        auto& io = ImGui::GetIO();
        if (key >= 0 && key < IM_ARRAYSIZE(io.KeysDown))
        {
                switch (action)
                {
                case GLFW_PRESS:
                        io.KeysDown[key] = true;
                        break;

                case GLFW_RELEASE:
                        io.KeysDown[key] = false;
                        break;
                }
        }

        io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
        io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
        io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
#ifdef LD_PLATFORM_WINDOWS
        io.KeySuper = false;
#else
        io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
#endif
        switch (action)
        {
        case GLFW_PRESS:
                Events::GetList<KeyPressEvent>().DispatchEvent(key, scancode);
                break;

        case GLFW_RELEASE:
                Events::GetList<KeyReleaseEvent>().DispatchEvent(key, scancode);
                break;
        }
}
void GLFWWindow::GLFW_MouseButtonCallback(GLFWwindow* wnd, int button, int action, int mods)
{
        auto* pSelf = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(wnd));
        auto& io = ImGui::GetIO();
        bool is_pressed = action == GLFW_PRESS;
        switch (button)
        {
        case GLFW_MOUSE_BUTTON_LEFT: io.MouseDown[0] = is_pressed; break;

        case GLFW_MOUSE_BUTTON_RIGHT: io.MouseDown[1] = is_pressed; break;

        case GLFW_MOUSE_BUTTON_MIDDLE: io.MouseDown[2] = is_pressed; break;

        case GLFW_MOUSE_BUTTON_4: io.MouseDown[3] = is_pressed; break;

        case GLFW_MOUSE_BUTTON_5: io.MouseDown[4] = is_pressed; break;
        }
        if (is_pressed)
                Events::GetList<MouseButtonPressEvent>().DispatchEvent(button);
        else
                Events::GetList<MouseButtonReleaseEvent>().DispatchEvent(button);
}
void GLFWWindow::GLFW_CursorPosCallback(GLFWwindow* wnd, double xpos, double ypos)
{
        auto* pSelf = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(wnd));
        auto& io = ImGui::GetIO();
        io.MousePos = ImVec2(static_cast<float>(xpos), static_cast<float>(ypos));
        Events::GetList<MouseMotionEvent>().DispatchEvent(xpos, ypos);
}
void GLFWWindow::GLFW_CharCallback(GLFWwindow* wnd, unsigned int c)
{
        auto* pSelf = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(wnd));
        auto& io = ImGui::GetIO();
        io.AddInputCharacter(c);
        Events::GetList<CharEvent>().DispatchEvent(c);
}

void GLFWWindow::GLFW_ScrollCallback(GLFWwindow* wnd, double xoffset, double yoffset)
{
        auto* pSelf = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(wnd));
        auto& io = ImGui::GetIO();
        io.MouseWheelH = xoffset;
        io.MouseWheel = yoffset;
        Events::GetList<MouseScrollEvent>().DispatchEvent(yoffset);
}

const char* GLFWWindow::ImGui_ImplGlfw_GetClipboardText(void* user_data)
{
        return glfwGetClipboardString((GLFWwindow*)user_data);
}

void GLFWWindow::ImGui_ImplGlfw_SetClipboardText(void* user_data, const char* text)
{
        glfwSetClipboardString((GLFWwindow*)user_data, text);
}
void GLFWWindow::GLFW_WindowCloseCallback(GLFWwindow* wnd)
{
        auto* pSelf = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(wnd));
        pSelf->Quit = true;
}
}
