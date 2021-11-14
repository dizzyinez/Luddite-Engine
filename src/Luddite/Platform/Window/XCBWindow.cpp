#include "Luddite/Core/pch.hpp"
#include <xcb/xcb.h>
#ifdef LD_PLATFORM_LINUX
#include "Luddite/Platform/Window/XCBWindow.hpp"
#include "Luddite/Core/Logging.hpp"
#include "Imgui/interface/ImGuiImplLinuxXCB.hpp"

enum XCB_SIZE_HINT
{
        XCB_SIZE_HINT_US_POSITION   = 1 << 0,
        XCB_SIZE_HINT_US_SIZE       = 1 << 1,
        XCB_SIZE_HINT_P_POSITION    = 1 << 2,
        XCB_SIZE_HINT_P_SIZE        = 1 << 3,
        XCB_SIZE_HINT_P_MIN_SIZE    = 1 << 4,
        XCB_SIZE_HINT_P_MAX_SIZE    = 1 << 5,
        XCB_SIZE_HINT_P_RESIZE_INC  = 1 << 6,
        XCB_SIZE_HINT_P_ASPECT      = 1 << 7,
        XCB_SIZE_HINT_BASE_SIZE     = 1 << 8,
        XCB_SIZE_HINT_P_WIN_GRAVITY = 1 << 9
};

struct xcb_size_hints_t
{
        uint32_t flags;                      /** User specified flags */
        int32_t x, y;                        /** User-specified position */
        int32_t width, height;               /** User-specified size */
        int32_t min_width, min_height;       /** Program-specified minimum size */
        int32_t max_width, max_height;       /** Program-specified maximum size */
        int32_t width_inc, height_inc;       /** Program-specified resize increments */
        int32_t min_aspect_num, min_aspect_den; /** Program-specified minimum aspect ratios */
        int32_t max_aspect_num, max_aspect_den; /** Program-specified maximum aspect ratios */
        int32_t base_width, base_height;     /** Program-specified base size */
        uint32_t win_gravity;                /** Program-specified window gravity */
};

namespace Luddite
{
XCBWindow::XCBWindow(const std::string& title, int width, int height, int min_width, int min_height)
{
        InitXCBConnectionAndWindow(title, width, height, min_width, min_height);
        InitVulkan();
        xcb_flush(info.connection);

        OnWindowResize(width, height);
}

bool XCBWindow::InitVulkan()
{
        {
                Diligent::SwapChainDesc SCDesc;
                Diligent::LinuxNativeWindow XCB_Window;
                XCB_Window.WindowId = info.window;
                XCB_Window.pXCBConnection = info.connection;
                Diligent::GetEngineFactoryVk()->CreateSwapChainVk(Renderer::GetDevice(), Renderer::GetContext(), SCDesc, XCB_Window, &m_pSwapChain);
        }

        auto& SCDesc = m_pSwapChain->GetDesc();
        Renderer::SetDefaultRTVFormat(SCDesc.ColorBufferFormat);
        Renderer::SetDefaultDSVFormat(SCDesc.DepthBufferFormat);
        m_pImGuiImpl.reset(new Diligent::ImGuiImplLinuxXCB(
                info.connection,
                Renderer::GetDevice(),
                SCDesc.ColorBufferFormat,
                SCDesc.DepthBufferFormat,
                SCDesc.Width,
                SCDesc.Height
                ));
        ImGuiSetup();
        m_pImGuiImpl->CreateDeviceObjects();
        return true;
}

bool XCBWindow::InitNativeEngineFactory()
{
        Diligent::EngineVkCreateInfo EngVkAttribs;
        EngVkAttribs.DebugMessageCallback = Logger::DiligentLogMessage;

        auto* pFactoryVk = Diligent::GetEngineFactoryVk();
        Renderer::GetEngineFactory() = pFactoryVk;
        pFactoryVk->CreateDeviceAndContextsVk(EngVkAttribs, &Renderer::GetDevice(), &Renderer::GetContext());
        return true;
}

void XCBWindow::InitXCBConnectionAndWindow(const std::string& title, int width, int height, int min_width, int min_height)
{
        int scr = 0;
        info.connection = xcb_connect(nullptr, &scr);
        LD_VERIFY(info.connection && xcb_connection_has_error(info.connection) == 0, "Unable to make an XCB connection, error code: {}", xcb_connection_has_error(info.connection));
        LD_LOG_INFO("XCB connection established");

        //Init XKB
        static uint8_t xkb_base_event;
        static uint8_t xkb_base_error;
        xkb_x11_setup_xkb_extension(info.connection,
                XKB_X11_MIN_MAJOR_XKB_VERSION,
                XKB_X11_MIN_MINOR_XKB_VERSION,
                XKB_X11_SETUP_XKB_EXTENSION_NO_FLAGS,
                NULL,
                NULL,
                &xkb_base_event,
                &xkb_base_error
                );
        kb_info.ctx = xkb_context_new(XKB_CONTEXT_NO_FLAGS);;
        LD_VERIFY(kb_info.ctx, "Unable to create XKB context");
        int32_t device_id;
        device_id = xkb_x11_get_core_keyboard_device_id(info.connection);
        LD_VERIFY(device_id > -1, "Couldn't get XKB keyboard device!");
        kb_info.keymap = xkb_x11_keymap_new_from_device(kb_info.ctx, info.connection, device_id, XKB_KEYMAP_COMPILE_NO_FLAGS);
        LD_VERIFY(kb_info.keymap, "Unable to create XKB keymap!");
        kb_info.state = xkb_state_new(kb_info.keymap);
        LD_VERIFY(kb_info.state, "Unable to create XKB state!");


        const xcb_setup_t*    setup = xcb_get_setup(info.connection);
        xcb_screen_iterator_t iter = xcb_setup_roots_iterator(setup);
        while (scr-- > 0)
                xcb_screen_next(&iter);

        auto screen = iter.data;

        info.width = width;
        info.height = height;

        uint32_t value_mask, value_list[32];

        info.window = xcb_generate_id(info.connection);

        value_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
        value_list[0] = screen->black_pixel;
        value_list[1] = XCB_EVENT_MASK_KEY_PRESS |
                        XCB_EVENT_MASK_KEY_RELEASE |
                        XCB_EVENT_MASK_EXPOSURE |
                        XCB_EVENT_MASK_STRUCTURE_NOTIFY |
                        XCB_EVENT_MASK_BUTTON_PRESS |
                        XCB_EVENT_MASK_BUTTON_RELEASE |
                        XCB_EVENT_MASK_POINTER_MOTION |
                        XCB_EVENT_MASK_BUTTON_MOTION |
                        XCB_EVENT_MASK_BUTTON_1_MOTION |
                        XCB_EVENT_MASK_BUTTON_2_MOTION |
                        XCB_EVENT_MASK_BUTTON_3_MOTION |
                        XCB_EVENT_MASK_BUTTON_4_MOTION |
                        XCB_EVENT_MASK_BUTTON_5_MOTION
        ;

        xcb_create_window(info.connection, XCB_COPY_FROM_PARENT, info.window, screen->root, 0, 0, info.width, info.height, 0,
                XCB_WINDOW_CLASS_INPUT_OUTPUT, screen->root_visual, value_mask, value_list);


        // Magic code that will send notification when window is destroyed
        xcb_intern_atom_cookie_t cookie = xcb_intern_atom(info.connection, 1, 12, "WM_PROTOCOLS");
        xcb_intern_atom_reply_t* reply = xcb_intern_atom_reply(info.connection, cookie, 0);

        xcb_intern_atom_cookie_t cookie2 = xcb_intern_atom(info.connection, 0, 16, "WM_DELETE_WINDOW");
        info.atom_wm_delete_window = xcb_intern_atom_reply(info.connection, cookie2, 0);

        xcb_change_property(info.connection, XCB_PROP_MODE_REPLACE, info.window, (*reply).atom, 4, 32, 1,
                &(*info.atom_wm_delete_window).atom);
        free(reply);

        SetTitle(title);

        // https://stackoverflow.com/a/27771295
        xcb_size_hints_t hints = {};
        hints.flags = XCB_SIZE_HINT_P_MIN_SIZE;
        hints.min_width = min_width;
        hints.min_height = min_height;
        xcb_change_property(info.connection, XCB_PROP_MODE_REPLACE, info.window, XCB_ATOM_WM_NORMAL_HINTS, XCB_ATOM_WM_SIZE_HINTS,
                32, sizeof(xcb_size_hints_t), &hints);

        xcb_map_window(info.connection, info.window);

        // Force the x/y coordinates to 100,100 results are identical in consecutive
        // runs
        const uint32_t coords[] = {100, 100};
        xcb_configure_window(info.connection, info.window, XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y, coords);
        xcb_flush(info.connection);

        xcb_generic_event_t* e;
        while ((e = xcb_wait_for_event(info.connection)))
        {
                if ((e->response_type & ~0x80) == XCB_EXPOSE) break;
        }
}

void XCBWindow::PollEvents()
{
        xcb_generic_event_t* event = nullptr;
        int16_t scrolls = 0;
        while ((event = xcb_poll_for_event(info.connection)) != nullptr)
        {
                bool imgui_handled = static_cast<Diligent::ImGuiImplLinuxXCB*>(m_pImGuiImpl.get())->HandleXCBEvent(event);
                // if (imgui_handled)
                //         continue;
                switch (event->response_type & 0x7f)
                {
                case XCB_CLIENT_MESSAGE:
                        if ((*(xcb_client_message_event_t*)event).data.data32[0] ==
                            (*info.atom_wm_delete_window).atom)
                        {
                                Quit = true;
                        }
                        break;

                case XCB_DESTROY_NOTIFY:
                        Quit = true;
                        break;

                case XCB_CONFIGURE_NOTIFY:
                {
                        const auto* cfgEvent = reinterpret_cast<const xcb_configure_notify_event_t*>(event);
                        if ((cfgEvent->width != info.width) || (cfgEvent->height != info.height))
                        {
                                info.width = cfgEvent->width;
                                info.height = cfgEvent->height;
                                if ((info.width > 0) && (info.height > 0))
                                {
                                        Events::GetList<WindowSizeEvent>().DispatchEvent(info.width, info.height);
                                }
                        }
                        break;
                }

                case XCB_KEY_PRESS:
                {
                        const auto* key_press = reinterpret_cast<const xcb_key_press_event_t*>(event);
                        xkb_keycode_t keycode = key_press->detail;
                        xkb_keysym_t keysym = xkb_state_key_get_one_sym(kb_info.state, keycode);
                        Events::GetList<KeyPressEvent>().DispatchEvent(keysym);
                        break;
                }

                case XCB_KEY_RELEASE:
                {
                        const auto* key_release = reinterpret_cast<const xcb_key_release_event_t*>(event);
                        xkb_keycode_t keycode = key_release->detail;
                        xkb_keysym_t keysym = xkb_state_key_get_one_sym(kb_info.state, keycode);
                        Events::GetList<KeyReleaseEvent>().DispatchEvent(keysym);
                        break;
                }

                case XCB_BUTTON_PRESS:
                {
                        const auto* mouse_press = reinterpret_cast<const xcb_button_press_event_t*>(event);
                        switch (mouse_press->detail)
                        {
                        case XCB_BUTTON_INDEX_4: scrolls += 1; break;

                        case XCB_BUTTON_INDEX_5: scrolls -= 1; break;
                        }
                        break;
                }

                case XCB_BUTTON_RELEASE:
                {
                        const auto* mouse_release = reinterpret_cast<const xcb_button_release_event_t*>(event);
                        Events::GetList<MouseButtonReleaseEvent>().DispatchEvent(mouse_release->detail);
                }
                }
                //Send input to application or whatever
                free(event);
        }
        if (scrolls != 0)
                Events::GetList<MouseScrollEvent>().DispatchEvent(scrolls);
}

void XCBWindow::SetTitle(const std::string& title)
{
        const char* title_pointer = title.data();
        xcb_change_property(info.connection, XCB_PROP_MODE_REPLACE, info.window, XCB_ATOM_WM_NAME, XCB_ATOM_STRING,
                8, title.size(), title.data());
        // const char* title_pointer = "GAMING";
        // xcb_change_property(info.connection, XCB_PROP_MODE_REPLACE, info.window, XCB_ATOM_WM_NAME, XCB_ATOM_STRING,
        //         8, strlen(title_pointer), title_pointer);
}

XCBWindow::~XCBWindow()
{
        xcb_destroy_window(info.connection, info.window);
        xcb_disconnect(info.connection);
}
}

#endif // LD_PLATFORM_LINUX
