#pragma once

#include "Luddite/Core/pch.hpp"
#ifdef LD_PLATFORM_WINDOWS
#include <windows.h>
#undef min
#undef max
#include "Luddite/Platform/Window/Window.hpp"
// #include "Luddite/Graphics/DiligentInclude.hpp"

namespace Luddite
{
class LUDDITE_API WindowsWindow : public Window
{
public:
        // WindowsWindow(const std::string& title, int width, int height, int min_width, int min_height);
        virtual ~WindowsWindow();

        void SetTitle(const std::string& title) override;

        void PollEvents() override;
protected:
        HWND m_WindowHandle = nullptr;
        int m_MinWidth = 320;
        int m_MinHeight = 240;
        void CreateWindowsWindow(const std::string& title, int width, int height, int min_width, int min_height);
        static LRESULT CALLBACK MessageProc(HWND wnd, UINT message, WPARAM wParam, LPARAM lParam);
        static inline std::unordered_map<HWND, WindowsWindow*> m_Windows;
        static inline WindowsWindow* m_MostRecentlyCreatedWindow = nullptr;
};
}

#endif // LD_PLATFORM_WINDOWS