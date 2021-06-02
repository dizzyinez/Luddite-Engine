#include "Luddite/Core/pch.hpp"
#ifdef LD_PLATFORM_WINDOWS
#include "Luddite/Platform/Window/D3D12Window.hpp"
#include "Imgui/interface/ImGuiImplWin32.hpp"

#include "Luddite/Graphics/DiligentInclude.hpp"
#include "Graphics/GraphicsEngineD3D12/interface/EngineFactoryD3D12.h"

using namespace Diligent;

namespace Luddite
{
D3D12Window::D3D12Window(const std::string& title, int width, int height, int min_width, int min_height)
{
        m_MinWidth = min_width;
        m_MinHeight = min_height;
        m_MostRecentlyCreatedWindow = static_cast<WindowsWindow*>(this);
        LD_LOG_INFO("creating d3d12 window");
        CreateWindowsWindow(title, width, height, min_width, min_height);


        EngineD3D12CreateInfo EngineCI;
        EngineCI.DebugMessageCallback = Logger::DiligentLogMessage;

        auto* pFactoryD3D12 = Diligent::GetEngineFactoryD3D12();
        Renderer::GetEngineFactory() = pFactoryD3D12;
        pFactoryD3D12->CreateDeviceAndContextsD3D12(EngineCI, &Renderer::GetDevice(), &Renderer::GetContext());

        Win32NativeWindow Window{m_WindowHandle};
        {
                SwapChainDesc SCDesc;
                pFactoryD3D12->CreateSwapChainD3D12(Renderer::GetDevice(), Renderer::GetContext(), SCDesc, FullScreenModeDesc{}, Window, &m_pSwapChain);
        }

        auto& SCDesc = m_pSwapChain->GetDesc();
        Renderer::SetDefaultRTVFormat(SCDesc.ColorBufferFormat);
        m_pImGuiImpl.reset(new Diligent::ImGuiImplWin32(
                m_WindowHandle,
                Renderer::GetDevice(),
                SCDesc.ColorBufferFormat,
                SCDesc.DepthBufferFormat
                ));
        m_pImGuiImpl->CreateDeviceObjects();
        OnWindowResize(SCDesc.Width, SCDesc.Height);

        LD_LOG_INFO("d3d12 window created");
}
D3D12Window::~D3D12Window()
{
}
}
#endif //LD_PLATFORM_WINDOWS