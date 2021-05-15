#pragma once
#include "Luddite/pch.hpp"
#include "Luddite/Platform/Window/Window.hpp"
#include <xcb/xcb.h>

#include "Luddite/Platform/DiligentPlatform.hpp"
#include "Graphics/GraphicsEngineVulkan/interface/EngineFactoryVk.h"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"
#include "Graphics/GraphicsEngine/interface/SwapChain.h"

#include "Common/interface/RefCntAutoPtr.hpp"


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
        XCBWindow();
        XCBWindow(const std::string& title);
        ~XCBWindow() override;
        void SetTitle(const std::string& title) override;
        uint32_t GetWidth() override {return info.width;}
        uint32_t GetHeight() override {return info.height;}
private:
        void InitXCBConnectionAndWindow(const std::string& title);
        bool InitVulkan();
        XCBInfo info;
};
}
