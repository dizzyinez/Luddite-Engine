#pragma once
#include "Luddite/Core/pch.hpp"
#ifdef LD_PLATFORM_LINUX
#include "Luddite/Platform/Window/Window.hpp"
#include <xcb/xcb.h>

#include "Luddite/Graphics/DiligentInclude.hpp"
#include "Graphics/GraphicsEngineVulkan/interface/EngineFactoryVk.h"

struct XCBInfo
{
        xcb_connection_t*        connection = nullptr;
        uint32_t window = 0;
        uint16_t width = 0;
        uint16_t height = 0;
        xcb_intern_atom_reply_t* atom_wm_delete_window = nullptr;
};

namespace Luddite
{
class LUDDITE_API XCBWindow : public Window
{
public:
        XCBWindow(const std::string& title = "Luddite Engine Application", int width = 1024, int height = 768, int min_width = 320, int min_height = 240);
        ~XCBWindow() override;
        void SetTitle(const std::string& title) override;
        void PollEvents() override;

        static bool InitNativeEngineFactory();
private:
        void InitXCBConnectionAndWindow(const std::string& title, int width, int height, int min_width, int min_height);
        bool InitVulkan();
        XCBInfo info;
};
}

#endif // LD_PLATFORM_LINUX