#pragma once

#include "Luddite/Core/pch.hpp"
#ifdef LD_PLATFORM_WINDOWS

#include "Luddite/Platform/Window/WindowsWindow.hpp"

#include "Luddite/Graphics/DiligentInclude.hpp"
#include "Graphics/GraphicsEngineD3D12/interface/EngineFactoryD3D12.h"

namespace Luddite
{
class LUDDITE_API D3D12Window : public WindowsWindow
{
public:
        D3D12Window(const std::string& title, int width, int height, int min_width, int min_height);
        ~D3D12Window() override;
};
}

#endif // LD_PLATFORM_WINDOWS