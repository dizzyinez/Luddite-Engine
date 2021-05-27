#pragma once

#include "Luddite/pch.hpp"
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
        GLXWindow(const std::string& title = "Luddite Engine Application", int width = 1024, int height = 768, int min_width = 320, int min_height = 240);
        ~GLXWindow() override;

        void SetTitle(const std::string& title) override;

        // uint32_t GetWidth() override {return info.width;}
        // uint32_t GetHeight() override {return info.height;}
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
