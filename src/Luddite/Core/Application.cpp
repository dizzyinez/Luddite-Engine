#include "Luddite/Core/Application.hpp"
#include "Luddite/Graphics/RenderTarget.hpp"
#include "Luddite/Platform/Window/NativeWindow.hpp"
#include "Luddite/Core/Assets.hpp"
#include "Luddite/Core/Profiler.hpp"

namespace Luddite
{
Application::Application()
{
        // m_RenderingBackend = RenderingBackend::VULKAN;
}
Application::~Application()
{}
void Application::Run()
{
        LD_VERIFY(m_pMainWindow, "Main window was never created! Call the CreateMainWindow function in the app's constructor");

        Renderer::Initialize();
        Assets::Initialize();

        // m_SystemTable.pRuntimeObjectSystem = new RuntimeObjectSystem;
        // if (!m_SystemTable.pRuntimeObjectSystem->Initialise(&m_RCCppLogger, &m_SystemTable))
        // // if (!m_SystemTable.pRuntimeObjectSystem->Initialise(&m_RCCppLogger, &m_SystemTable))
        // {
        //         delete m_SystemTable.pRuntimeObjectSystem;
        //         m_SystemTable.pRuntimeObjectSystem = nullptr;
        //         LD_LOG_CRITICAL("Couldn't Create Game Instance");
        // }
        // m_SystemTable.pRuntimeObjectSystem->CleanObjectFiles();

// #ifndef LD_PLATFORM_WINDOWS
//         m_SystemTable.pRuntimeObjectSystem->SetAdditionalCompileOptions("-std=c++17");
// #endif //LD_PLATFORM_WINDOWS

        // ensure include directories are set - use location of this file as starting point
        // FileSystemUtils::Path basePath = m_SystemTable.pRuntimeObjectSystem->FindFile(__FILE__).ParentPath();
        // FileSystemUtils::Path imguiIncludeDir = basePath / "imgui";
        // m_SystemTable.pRuntimeObjectSystem->AddIncludeDir(imguiIncludeDir.c_str());


        std::chrono::microseconds min_update_time(1000000 / 60);
        std::chrono::duration<double> update_delta_time(min_update_time);
        // std::chrono::microseconds min_render_time(1000000 / 300); //300 max fps
        std::chrono::microseconds min_render_time(0); //unlimited fps

        std::chrono::microseconds update_accululator(0);
        std::chrono::microseconds render_accululator(0);
        // m_pMainWindow->m_Vsync = true;

        Initialize();

        double fixed_dt = std::chrono::duration_cast<std::chrono::duration<double> >(min_update_time).count();
        double delta_time = fixed_dt; //First frame will use fixed dt
        uint32_t frame_count = 0;
        while (!m_pMainWindow->ShouldQuit())
        {
                std::stringstream ss;
                ss << "Frame " << frame_count;
                LD_PROFILE_SCOPE(ss.str());
                frame_count++;
                std::chrono::high_resolution_clock::time_point loop_start = std::chrono::high_resolution_clock::now();

                // if (m_SystemTable.pRuntimeObjectSystem->GetIsCompiledComplete())
                // {
                //         m_SystemTable.pRuntimeObjectSystem->LoadCompiledModule();
                // }

                // if (!m_SystemTable.pRuntimeObjectSystem->GetIsCompiling())
                // {
                //         m_SystemTable.pRuntimeObjectSystem->GetFileChangeNotifier()->Update(delta_time);
                // }

                // if (m_SystemTable.pGameInstanceI)
                // m_SystemTable.pGameInstanceI->Initialize();


                //TEMP

                //event handling
                Events::Clear();
                m_pMainWindow->PollEvents();
                m_pMainWindow->HandleEvents();
                OnUpdate(delta_time);
                while (update_accululator > min_update_time)
                {
                        update_accululator -= min_update_time;
			OnFixedUpdate(fixed_dt);
                        //update
                        // m_pMainWindow->GetLayerStack().UpdateLayers(0.016667f);
                }

                //TEMP
                float lerp_alpha = 1.0f;

                Assets::MergeLoadedAssets();
                Assets::RefreshAssets();

                //render
                auto MainWindowRenderTarget = m_pMainWindow->GetRenderTarget();
                Renderer::BindRenderTarget(MainWindowRenderTarget);
                Renderer::ClearRenderTarget(MainWindowRenderTarget);
                OnRender(lerp_alpha);
                // m_pMainWindow->GetLayerStack().RenderLayers(1.0f, MainWindowRenderTarget);


                // while (render_accululator > min_render_time)
                // {
                // render_accululator -= min_render_time;
                //imgui render
                Renderer::BindRenderTarget(MainWindowRenderTarget);
                m_pMainWindow->ImGuiNewFrame();
                ImGuizmo::BeginFrame();
                OnImGuiRender(lerp_alpha);
                // m_pMainWindow->GetLayerStack().RenderLayersImGui(1.0f, MainWindowRenderTarget);
                m_pMainWindow->GetImGuiImpl()->Render(Renderer::m_pImmediateContext);
                {
                        LD_PROFILE_SCOPE("Swapping Window Buffers");
                        m_pMainWindow->SwapBuffers();
                }
                // }

                std::chrono::microseconds loop_time_span = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - loop_start);
                update_accululator += loop_time_span;
                render_accululator += loop_time_span;
                // render_accumulator
                delta_time = std::chrono::duration_cast<std::chrono::duration<double> >(loop_time_span).count();
                float fps = 1 / delta_time;
                // LD_LOG_INFO("fps: {}", fps);
                std::chrono::microseconds sec(1);
                // LD_LOG_INFO("elapsed: {}", update_accululator.count());
                // std::chrono::microseconds µs;
                // LD_LOG_INFO("DT: {}", update_delta_time.count());
        }
}
void Application::CreateMainWindow(const std::string& Name, int width, int height, int min_width, int min_height)
{
        //TEMP
        NativeVulkanWindow::InitNativeEngineFactory();
        m_pMainWindow = std::make_shared<NativeVulkanWindow>(Name, width, height, min_width, min_height);

        // m_pMainWindow = std::make_shared<NativeOpenGLWindow>(Name, width, height, min_width, min_height);

        // m_pMainWindow = std::make_shared<NativeD3D12Window>(Name, width, height, min_width, min_height);
}
}
