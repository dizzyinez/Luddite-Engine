#include "Luddite/Core/pch.hpp"
#ifdef LD_PLATFORM_LINUX
#include "Luddite/Platform/Window/GLXWindow.hpp"
#include "Imgui/interface/ImGuiImplLinuxX11.hpp"

#ifndef GLX_CONTEXT_MAJOR_VERSION_ARB
#    define GLX_CONTEXT_MAJOR_VERSION_ARB 0x2091
#endif

#ifndef GLX_CONTEXT_MINOR_VERSION_ARB
#    define GLX_CONTEXT_MINOR_VERSION_ARB 0x2092
#endif

#ifndef GLX_CONTEXT_FLAGS_ARB
#    define GLX_CONTEXT_FLAGS_ARB 0x2094
#endif

#ifndef GLX_CONTEXT_DEBUG_BIT_ARB
#    define GLX_CONTEXT_DEBUG_BIT_ARB 0x0001
#endif

using namespace Diligent;

namespace Luddite
{
GLXWindow::GLXWindow(const std::string& title, int width, int height, int min_width, int min_height)
{
        InitGLXWindow(title, width, height, min_width, min_height);
}

GLXWindow::~GLXWindow()
{
        m_Context = glXGetCurrentContext();
        glXMakeCurrent(m_pDisplay, None, NULL);
        glXDestroyContext(m_pDisplay, m_Context);
        XDestroyWindow(m_pDisplay, m_Window);
        XCloseDisplay(m_pDisplay);
}

void GLXWindow::SetTitle(const std::string& title)
{
        LD_LOG_WARN("Set title not implemented for glx");
}

bool GLXWindow::InitOpenGL()
{
}

void GLXWindow::PollEvents()
{
        XEvent xev;
        // Handle all events in the queue
        while (XCheckMaskEvent(m_pDisplay, 0xFFFFFFFF, &xev))
        {
                bool imgui_handled = static_cast<Diligent::ImGuiImplLinuxX11*>(m_pImGuiImpl.get())->HandleXEvent(&xev);
                if (imgui_handled)
                        continue;

                switch (xev.type)
                {
                case KeyPress:
                {
                        KeySym keysym;
                        char buffer[80];
                        int num_char = XLookupString((XKeyEvent*)&xev, buffer, _countof(buffer), &keysym, 0);
                        (void)num_char;
                        // EscPressed = (keysym == XK_Escape);
                }

                case ConfigureNotify:
                {
                        XConfigureEvent& xce = reinterpret_cast<XConfigureEvent&>(xev);
                        if (xce.width != 0 && xce.height != 0)
                                OnWindowResize(xce.width, xce.height);
                        // TheApp->WindowResize(xce.width, xce.height);
                        break;
                }

                case DestroyNotify:
                {
                        Quit = true;
                        break;
                }
                }
        }
}

// bool GLXWindow::InitNativeEngineFactory()
// {
//         // auto* pFactoryOpenGL = GetEngineFactoryOpenGL();
//         // Renderer::GetEngineFactory() = pFactoryOpenGL;
//         // pFactoryOpenGL->CreateDeviceAndSwapChainGL


//         // EngineGLCreateInfo EngineCI;
//         // EngineCI.Window.WindowId = NativeWindowHandle;
//         // EngineCI.Window.pDisplay = m_pDisplay;

//         // pFactoryOpenGL->CreateDeviceAndSwapChainGL(EngineCI, &m_pDevice, &m_pImmediateContext, SCDesc, &m_pSwapChain);
// }

void GLXWindow::InitGLXWindow(const std::string& title, int width, int height, int min_width, int min_height)
{
        m_pDisplay = XOpenDisplay(0);

        static int visual_attribs[] =
        {
                GLX_RENDER_TYPE, GLX_RGBA_BIT,
                GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
                GLX_DOUBLEBUFFER, true,

                // The largest available total RGBA color buffer size (sum of GLX_RED_SIZE,
                // GLX_GREEN_SIZE, GLX_BLUE_SIZE, and GLX_ALPHA_SIZE) of at least the minimum
                // size specified for each color component is preferred.
                GLX_RED_SIZE, 8,
                GLX_GREEN_SIZE, 8,
                GLX_BLUE_SIZE, 8,
                GLX_ALPHA_SIZE, 8,

                // The largest available depth buffer of at least GLX_DEPTH_SIZE size is preferred
                GLX_DEPTH_SIZE, 24,

                //GLX_SAMPLE_BUFFERS, 1,
                GLX_SAMPLES, 1,
                None
        };

        int fbcount = 0;
        GLXFBConfig* fbc = glXChooseFBConfig(m_pDisplay, DefaultScreen(m_pDisplay), visual_attribs, &fbcount);
        if (!fbc)
        {
                LD_LOG_ERROR("Failed to retrieve a framebuffer config");
                exit(-1);
        }

        XVisualInfo* vi = glXGetVisualFromFBConfig(m_pDisplay, fbc[0]);

        XSetWindowAttributes swa;
        swa.colormap = XCreateColormap(m_pDisplay, RootWindow(m_pDisplay, vi->screen), vi->visual, AllocNone);
        swa.border_pixel = 0;
        swa.event_mask =
                // EnterWindowMask |
                // LeaveWindowMask |
                StructureNotifyMask |
                ExposureMask |
                KeyPressMask |
                KeyReleaseMask |
                ButtonPressMask |
                ButtonReleaseMask |
                PointerMotionMask;

        m_Window = XCreateWindow(m_pDisplay, RootWindow(m_pDisplay, vi->screen), 0, 0, width, height, 0, vi->depth, InputOutput, vi->visual, CWBorderPixel | CWColormap | CWEventMask, &swa);
        if (!m_Window)
        {
                LD_LOG_ERROR("Failed to create window");
                exit(-1);
        }

        {
                auto SizeHints = XAllocSizeHints();
                SizeHints->flags = PMinSize;
                SizeHints->min_width = min_width;
                SizeHints->min_height = min_height;
                XSetWMNormalHints(m_pDisplay, m_Window, SizeHints);
                XFree(SizeHints);
        }

        XMapWindow(m_pDisplay, m_Window);

        glXCreateContextAttribsARBProc glXCreateContextAttribsARB = nullptr;
        {
                // Create an oldstyle context first, to get the correct function pointer for glXCreateContextAttribsARB
                GLXContext ctx_old = glXCreateContext(m_pDisplay, vi, 0, GL_TRUE);
                glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddress((const GLubyte*)"glXCreateContextAttribsARB");
                glXMakeCurrent(m_pDisplay, None, NULL);
                glXDestroyContext(m_pDisplay, ctx_old);
        }

        if (glXCreateContextAttribsARB == nullptr)
        {
                LD_LOG_ERROR("glXCreateContextAttribsARB entry point not found");
                exit(-1);
        }

        int Flags = GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;
        #ifdef LD_DEBUG
        Flags |= GLX_CONTEXT_DEBUG_BIT_ARB;
        #endif

        int major_version = 4;
        int minor_version = 3;
        static int context_attribs[] =
        {
                GLX_CONTEXT_MAJOR_VERSION_ARB, major_version,
                GLX_CONTEXT_MINOR_VERSION_ARB, minor_version,
                GLX_CONTEXT_FLAGS_ARB, Flags,
                None
        };

        m_Context = glXCreateContextAttribsARB(m_pDisplay, fbc[0], NULL, 1, context_attribs);
        if (!m_Context)
        {
                LD_LOG_ERROR("Failed to create GL context");
                exit(-1);
        }
        XFree(fbc);


        glXMakeCurrent(m_pDisplay, m_Window, m_Context);

        {
                SwapChainDesc SCDesc;
                // Declare function pointer
                auto* pFactoryOpenGL = GetEngineFactoryOpenGL();
                Renderer::GetEngineFactory() = pFactoryOpenGL;

                EngineGLCreateInfo EngineCI;
                EngineCI.DebugMessageCallback = Logger::DiligentLogMessage;
                EngineCI.Window.WindowId = m_Window;
                EngineCI.Window.pDisplay = m_pDisplay;

                pFactoryOpenGL->CreateDeviceAndSwapChainGL(EngineCI, &Renderer::GetDevice(), &Renderer::GetContext(), SCDesc, &m_pSwapChain);
        }

        auto& SCDesc = m_pSwapChain->GetDesc();
        Renderer::SetDefaultRTVFormat(SCDesc.ColorBufferFormat);
        m_pImGuiImpl.reset(         //std::make_unique<Diligent::ImGuiImplDiligent>(
                new Diligent::ImGuiImplLinuxX11(Renderer::GetDevice(),
                        SCDesc.ColorBufferFormat,
                        SCDesc.DepthBufferFormat,
                        SCDesc.Width,
                        SCDesc.Height
                        ));
        m_pImGuiImpl->CreateDeviceObjects();


        XStoreName(m_pDisplay, m_Window, title.c_str());
}
}

#endif // LD_PLATFORM_LINUX