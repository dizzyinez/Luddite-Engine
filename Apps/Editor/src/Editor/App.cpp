#define LD_ENTRYPOINT
#include "Luddite/Core/EntryPoint.hpp"
#include "Editor/pch.hpp"
#include "Editor/Layers/EditorLayer.hpp"

class Editor : public Luddite::Application
{
        public:
        Editor()
        {
                LD_LOG_INFO("Test Application starting");
                CreateMainWindow("Luddite Test App");
                auto layer = std::make_shared<EditorLayer>();
                m_MainWindow->GetLayerStack().PushLayer(layer);
                LD_LOG_INFO("Window Created");
        }
        ~Editor()
        {
        }
};

Luddite::Application* Luddite::CreateApplication()
{
        return new Editor();
}