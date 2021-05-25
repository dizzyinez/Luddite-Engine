#include "Luddite/Luddite.hpp"
#include "Luddite/Platform/Window/XCBWindow.hpp"
#include "AppLayer.hpp"

class TestApp : public Luddite::Application
{
public:
        TestApp()
        {
                LD_LOG_INFO("Test Application starting");
                CreateMainWindow("Luddite Test App");
                // auto game_window = std::make_shared<Luddite::XCBWindow>("Luddite Test App");
                // auto game_window2 = std::make_shared<Luddite::XCBWindow>("Luddite Test App");
                // game_window->SetTitle("GAMING TIME");
                // m_windows.emplace_back(game_window);
                // m_windows.emplace_back(game_window2);
                auto layer = std::make_shared<AppLayer>();
                m_MainWindow->GetLayerStack().PushLayer(layer);
                LD_LOG_INFO("Window Created");
        }
        ~TestApp()
        {}
};

Luddite::Application* Luddite::CreateApplication()
{
        return new TestApp();
}
