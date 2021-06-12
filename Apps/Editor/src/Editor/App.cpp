#define LD_ENTRYPOINT
#include "Luddite/Core/EntryPoint.hpp"
#include "Editor/pch.hpp"
#include "Editor/Layers/EditorLayer.hpp"

#include "RuntimeObjectSystem.h"

#include "RCCppLogger.hpp"
#include "SystemTable.h"
#include "Editor/GameInstance.hpp"

static RCCppLogger g_Logger;
static SystemTable g_SystemTable;

class Editor : public Luddite::Application
{
        public:
        Editor()
        {
                LD_LOG_INFO("Test Application starting");
                CreateMainWindow("Luddite Test App");
                auto layer = std::make_shared<EditorLayer>();
                m_pMainWindow->GetLayerStack().PushLayer(layer);
                LD_LOG_INFO("Window Created");
                InitRCCpp();
        }
        ~Editor()
        {
        }

        void OnUpdate()
        {
        }

        void OnRender()
        {
        }

        void OnImGuiRender()
        {
        }

        bool InitRCCpp()
        {
                g_SystemTable.pRuntimeObjectSystem = new RuntimeObjectSystem;
                if (!g_SystemTable.pRuntimeObjectSystem->Initialise(&g_Logger, &g_SystemTable))
                {
                        delete g_SystemTable.pRuntimeObjectSystem;
                        g_SystemTable.pRuntimeObjectSystem = 0;
                        return false;
                }

                g_SystemTable.pRuntimeObjectSystem->CleanObjectFiles();
#ifndef LD_PLATFORM_WINDOWS
                g_SystemTable.pRuntimeObjectSystem->SetAdditionalCompileOptions("-std=c++11");
#endif

                // ensure include directories are set - use location of this file as starting point
                FileSystemUtils::Path basePath = g_SystemTable.pRuntimeObjectSystem->FindFile(__FILE__).ParentPath();
                FileSystemUtils::Path imguiIncludeDir = basePath / "imgui";
                g_SystemTable.pRuntimeObjectSystem->AddIncludeDir(imguiIncludeDir.c_str());

                return true;
        }

        void UpdateRCCpp()
        {
        }

        void CleanRCCpp()
        {
        }
};

Luddite::Application* Luddite::CreateApplication()
{
        return new Editor();
}