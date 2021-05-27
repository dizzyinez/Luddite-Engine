#include "Application.hpp"
//TEMP
#include "Luddite/Platform/Window/NativeWindow.hpp"

namespace Luddite
{
Application::Application()
{
        m_RenderingBackend = RenderingBackend::VULKAN;
}
Application::~Application()
{}
void Application::Run()
{
        #ifdef LD_DEBUG
        if (!m_MainWindow)
        {
                LD_LOG_CRITICAL("Main window was never created !Call the CreateMainWindow function in the app's constructor.");
                exit(0);
        }
        #endif // LD_DEBUG

        Renderer::Initialize();
        auto rt = Renderer::CreateRenderTexture(800, 600);

        while (!m_MainWindow->ShouldQuit())
        {
                m_MainWindow->HandleEvents();
                // m_Renderer.Draw();
                // m_Renderer.Present();

                auto MainWindowRenderTarget = m_MainWindow->GetRenderTarget();
                // Renderer::TransitionRenderTextureToRenderTarget(rt);
                // Renderer::Draw(rt.GetRenderTarget());


                // Renderer::TransitionRenderTextureToShaderResource(rt);


                Renderer::BindRenderTarget(MainWindowRenderTarget);
                Renderer::ClearRenderTarget(MainWindowRenderTarget);
                Renderer::Draw(MainWindowRenderTarget);
                Renderer::Draw(rt.GetRenderTarget());

                Renderer::BindRenderTarget(MainWindowRenderTarget);
                m_MainWindow->ImGuiNewFrame();
                ImGui::Begin("Viewport");
                ImGui::Image(rt.GetShaderResourceView(), ImVec2(rt.GetRenderTarget().width, rt.GetRenderTarget().height));
                ImGui::End();
                m_MainWindow->GetImGuiImpl()->Render(Renderer::m_pImmediateContext);

                // ImGui::Begin("Viewport2");
                // ImGui::Image(rt.GetShaderResourceView(), ImVec2(rt.GetRenderTarget().width, rt.GetRenderTarget().height));
                // ImGui::End();
                // ImGui::Begin("Viewport3");
                // ImGui::Image(rt.GetShaderResourceView(), ImVec2(rt.GetRenderTarget().width, rt.GetRenderTarget().height));
                // ImGui::End();
                // m_MainWindow->
                m_MainWindow->SwapBuffers();
        }
}
void Application::CreateMainWindow(const std::string& Name, int width, int height)
{
        //TEMP
        // NativeVulkanWindow::InitNativeEngineFactory();
        // m_MainWindow = std::make_shared<NativeVulkanWindow>(Name, width, height);

        m_MainWindow = std::make_shared<NativeOpenGLWindow>(Name);
}
}
