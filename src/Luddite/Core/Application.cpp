#include "Luddite/Core/Application.hpp"
#include "Luddite/Graphics/RenderTarget.hpp"
#include "Luddite/Platform/Window/NativeWindow.hpp"

#include "Luddite/Core/RCCppLogger.hpp"
#include "Luddite/Core/SystemTable.h"
#include "Luddite/Core/GameInstance.hpp"

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
        LD_VERIFY(m_MainWindow, "Main window was never created! Call the CreateMainWindow function in the app's constructor");

        Renderer::Initialize();

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
        std::chrono::microseconds min_render_time(1000000 / 300); //300 is max fps

        std::chrono::microseconds update_accululator(0);
        std::chrono::microseconds render_accululator(0);
        m_MainWindow->m_Vsync = true;
        double delta_time;
        while (!m_MainWindow->ShouldQuit())
        {
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




                //event handling
                m_MainWindow->ClearEvents();
                m_MainWindow->PollEvents();
                m_MainWindow->HandleEvents();
                while (update_accululator > min_update_time)
                {
                        update_accululator -= min_update_time;
                        //update
                        OnUpdate();
                        // m_MainWindow->GetLayerStack().UpdateLayers(0.016667f);
                }


                //render
                auto MainWindowRenderTarget = m_MainWindow->GetRenderTarget();
                Renderer::BindRenderTarget(MainWindowRenderTarget);
                Renderer::ClearRenderTarget(MainWindowRenderTarget);
                OnRender();
                // m_MainWindow->GetLayerStack().RenderLayers(1.0f, MainWindowRenderTarget);


                // while (render_accululator > min_render_time)
                // {
                // render_accululator -= min_render_time;
                //imgui render
                Renderer::BindRenderTarget(MainWindowRenderTarget);
                m_MainWindow->ImGuiNewFrame();
                ImGuizmo::BeginFrame();
                OnImGuiRender();
                // m_MainWindow->GetLayerStack().RenderLayersImGui(1.0f, MainWindowRenderTarget);
                m_MainWindow->GetImGuiImpl()->Render(Renderer::m_pImmediateContext);
                m_MainWindow->SwapBuffers();
                // }

                std::chrono::microseconds loop_time_span = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - loop_start);
                update_accululator += loop_time_span;
                render_accululator += loop_time_span;
                // render_accululator
                delta_time = std::chrono::duration_cast<std::chrono::duration<double> >(loop_time_span).count();
                // LD_LOG_INFO("fps: {}", fps);
                // LD_LOG_INFO("elapsed: {}", update_accululator.count());
                // std::chrono::microseconds µs;
                std::chrono::microseconds sec(1);
                // LD_LOG_INFO("DT: {}", update_delta_time.count());
        }
}
void Application::CreateMainWindow(const std::string& Name, int width, int height, int min_width, int min_height)
{
        //TEMP
        NativeVulkanWindow::InitNativeEngineFactory();
        m_MainWindow = std::make_shared<NativeVulkanWindow>(Name, width, height, min_width, min_height);

        // m_MainWindow = std::make_shared<NativeOpenGLWindow>(Name, width, height, min_width, min_height);

        // m_MainWindow = std::make_shared<NativeD3D12Window>(Name, width, height, min_width, min_height);
}
}
