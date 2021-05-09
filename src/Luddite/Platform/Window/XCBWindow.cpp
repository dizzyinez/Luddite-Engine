#include "Luddite/Platform/Window/XCBWindow.hpp"
#include "Luddite/Logging.hpp"
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
XCBWindow::XCBWindow()
{
        InitXCBConnectionAndWindow();
        InitVulkan();
        xcb_flush(info.connection);
}

bool XCBWindow::InitVulkan()
{
        Diligent::EngineVkCreateInfo EngVkAttribs;

        auto* pFactoryVk = Diligent::GetEngineFactoryVk();
        pFactoryVk->CreateDeviceAndContextsVk(EngVkAttribs, &GetRenderDevice(), &GetDeviceContext());
        Diligent::SwapChainDesc SCDesc;
        Diligent::LinuxNativeWindow XCB_Window;
        XCB_Window.WindowId = info.window;
        XCB_Window.pXCBConnection = info.connection;
        pFactoryVk->CreateSwapChainVk(GetRenderDevice(), GetDeviceContext(), SCDesc, XCB_Window, &GetSwapChain());

        return true;
}

void XCBWindow::InitXCBConnectionAndWindow()
{
        int scr = 0;
        info.connection = xcb_connect(nullptr, &scr);
        if (info.connection == nullptr || xcb_connection_has_error(info.connection))
        {
                LOG_CRITICAL("Unable to make an XCB connection");
        }
        LOG_INFO("XCB connection established");

        const xcb_setup_t*    setup = xcb_get_setup(info.connection);
        xcb_screen_iterator_t iter = xcb_setup_roots_iterator(setup);
        while (scr-- > 0)
                xcb_screen_next(&iter);

        auto screen = iter.data;

        info.width = 1024;
        info.height = 768;

        uint32_t value_mask, value_list[32];

        info.window = xcb_generate_id(info.connection);

        value_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
        value_list[0] = screen->black_pixel;
        value_list[1] = XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_STRUCTURE_NOTIFY;

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


        // const char* title = "Tutorial00: Hello Linux (Vulkan)";
        // xcb_change_property(info.connection, XCB_PROP_MODE_REPLACE, info.window, XCB_ATOM_WM_NAME, XCB_ATOM_STRING,
        //         8, strlen(title), title);

        // https://stackoverflow.com/a/27771295
        xcb_size_hints_t hints = {};
        hints.flags = XCB_SIZE_HINT_P_MIN_SIZE;
        hints.min_width = 320;
        hints.min_height = 240;
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

void XCBWindow::SetTitle(std::string title)
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