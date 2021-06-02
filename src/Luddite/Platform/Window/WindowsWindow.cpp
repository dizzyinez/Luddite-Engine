#include "Luddite/Core/pch.hpp"
#ifdef LD_PLATFORM_WINDOWS
#include "Luddite/Platform/Window/WindowsWindow.hpp"
#include "Imgui/interface/ImGuiImplWin32.hpp"
#include "Luddite/Core/core.hpp"
#include <winuser.h>

using namespace Diligent;

namespace Luddite
{
// WindowsWindow::WindowsWindow(const std::string& title, int width, int height, int min_width, int min_height)
// {
// }
WindowsWindow::~WindowsWindow()
{
        DestroyWindow(m_WindowHandle);
}

void WindowsWindow::SetTitle(const std::string& title)
{
}

void WindowsWindow::CreateWindowsWindow(const std::string& title, int width, int height, int min_width, int min_height)
{
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t> > converter;
        std::wstring wide_title = converter.from_bytes(title);

        // Register our window class
        WNDCLASSEX wcex = {sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW, MessageProc,
                           0L, 0L, WindowsProperties::instance, NULL, NULL, NULL, NULL, L"Luddite Window Class", NULL};
        RegisterClassEx(&wcex);

        // Create a window
        RECT rc = {0, 0, width, height};
        AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
        m_WindowHandle = CreateWindowExW(0, L"Luddite Window Class", wide_title.c_str(),
                WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, WindowsProperties::instance, NULL);
        LD_VERIFY(m_WindowHandle, "Failed to create Window");
        ShowWindow(m_WindowHandle, WindowsProperties::cmdShow);
        UpdateWindow(m_WindowHandle);
}

void WindowsWindow::PollEvents()
{
        MSG msg = { };
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0)
        {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
        }
}

LRESULT CALLBACK WindowsWindow::MessageProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
        WindowsWindow* window = nullptr;
        for (auto& pair : m_Windows)
        {
                if (pair.second->m_WindowHandle == hWnd)
                {
                        window = pair.second;
                        break;
                }
        }
        if (!window)
        {
                LD_VERIFY(m_MostRecentlyCreatedWindow, "WindowsWindow::m_MostRecentlyCreatedWindow was not set in native window initialization!");
                window = m_MostRecentlyCreatedWindow;
                WindowsWindow::m_Windows[hWnd] = m_MostRecentlyCreatedWindow;
        }
        if (window->m_pImGuiImpl)
        {
                auto ret = static_cast<Diligent::ImGuiImplWin32*>(window->m_pImGuiImpl.get())->Win32_ProcHandler(hWnd, message, wParam, lParam);
                if (ret != 0)
                        return ret;
        }
        switch (message)
        {
        case WM_CREATE:
                return 0;

        case WM_PAINT:
        {
                PAINTSTRUCT ps;
                BeginPaint(hWnd, &ps);
                EndPaint(hWnd, &ps);
                return 0;
        }

        case WM_SIZE:         // Window size has been changed
                window->m_EventPool.GetList<WindowSizeEvent>().DispatchEvent((unsigned int)LOWORD(lParam), (unsigned int)HIWORD(lParam));
                // window->OnWindowResize();
                return 0;

        // case WM_CHAR:
        //         if (wParam == VK_ESCAPE)
        //                 PostQuitMessage(0);
        //         return 0;

        case WM_KILLFOCUS:
                return 0;

        case WM_CLOSE:
        case WM_DESTROY:
                window->Quit = true;
                // PostQuitMessage(0);
                return 0;

        case WM_GETMINMAXINFO:
        {
                LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
                lpMMI->ptMinTrackSize.x = window->m_MinWidth;
                lpMMI->ptMinTrackSize.y = window->m_MinHeight;
                return 0;
        }

        default:
                return DefWindowProc(hWnd, message, wParam, lParam);
        }
}
}
#endif //LD_PLATFORM_WINDOWS