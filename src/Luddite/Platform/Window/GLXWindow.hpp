#pragma once

#include "Luddite/Core/pch.hpp"
#ifdef LD_PLATFORM_LINUX

#include "Luddite/Platform/Window/Window.hpp"
#include <GL/glx.h>
#include <GL/gl.h>

#include "Luddite/Graphics/DiligentInclude.hpp"
#include "Graphics/GraphicsEngineOpenGL/interface/EngineFactoryOpenGL.h"

typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, int, const int*);

namespace Luddite
{
class LUDDITE_API GLXWindow : public Window
{
public:
        GLXWindow(const std::string& title, int width, int height, int min_width, int min_height);
        ~GLXWindow() override;

        void SetTitle(const std::string& title) override;

        void HandleEvents() override;

        static bool InitNativeEngineFactory();
private:
        void InitGLXWindow(const std::string& title, int width, int height, int min_width, int min_height);
        bool InitOpenGL();
        Display* m_pDisplay;
        GLXContext m_Context;
        XID m_Window;
};
}

#endif // LD_PLATFORM_LINUX