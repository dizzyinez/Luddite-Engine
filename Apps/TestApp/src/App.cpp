#define LD_ENTRYPOINT
#include <Luddite/Luddite.hpp>
#include "AppLayer.hpp"

class TestApp : public Luddite::Application
{
public:
        TestApp()
        {
                LD_LOG_INFO("Test Application starting");
                CreateMainWindow("Luddite Test App");
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