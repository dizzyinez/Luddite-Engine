#include "Luddite/Core/Application.hpp"
#include "Luddite/Graphics/RenderTarget.hpp"
#include "Luddite/Platform/Window/NativeWindow.hpp"

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
        auto rt = Renderer::CreateRenderTexture(800, 600);

        Camera camera;
        camera.Position = glm::vec3(0.f, 0.f, -5.f);
        float pitch = 0.f;
        float yaw = 0.f;


        std::chrono::microseconds min_update_time(1000000 / 60);
        std::chrono::duration<double> update_delta_time(min_update_time);
        std::chrono::microseconds min_render_time(1000000 / 300); //300 is max fps

        std::chrono::microseconds update_accululator(0);
        std::chrono::microseconds render_accululator(0);
        m_MainWindow->m_Vsync = true;
        while (!m_MainWindow->ShouldQuit())
        {
                std::chrono::high_resolution_clock::time_point loop_start = std::chrono::high_resolution_clock::now();

                //event handling
                m_MainWindow->ClearEvents();
                m_MainWindow->PollEvents();
                m_MainWindow->HandleEvents();
                while (update_accululator > min_update_time)
                {
                        update_accululator -= min_update_time;
                        //update
                        m_MainWindow->GetLayerStack().UpdateLayers(0.016667f);
                }

                //render
                auto MainWindowRenderTarget = m_MainWindow->GetRenderTarget();
                Renderer::BindRenderTarget(MainWindowRenderTarget);
                Renderer::ClearRenderTarget(MainWindowRenderTarget);
                m_MainWindow->GetLayerStack().RenderLayers(1.0f, MainWindowRenderTarget);


                // while (render_accululator > min_render_time)
                // {
                // render_accululator -= min_render_time;
                //imgui render
                Renderer::BindRenderTarget(MainWindowRenderTarget);
                m_MainWindow->ImGuiNewFrame();
                m_MainWindow->GetLayerStack().RenderLayersImGui(1.0f, MainWindowRenderTarget);
                m_MainWindow->GetImGuiImpl()->Render(Renderer::m_pImmediateContext);
                m_MainWindow->SwapBuffers();
                // }

                std::chrono::microseconds loop_time_span = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - loop_start);
                update_accululator += loop_time_span;
                render_accululator += loop_time_span;
                // render_accululator
                double fps = 1.f / std::chrono::duration_cast<std::chrono::duration<double> >(loop_time_span).count();
                LD_LOG_INFO("fps: {}", fps);
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
