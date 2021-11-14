#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/Core/Core.hpp"
#include "Luddite/Platform/Window/Window.hpp"
#include "Luddite/Graphics/Renderer.hpp"

namespace Luddite
{
enum class RenderingBackend : uint8_t
{
        NONE,
        VULKAN,
        OPENGL,
        D3D11,
        D3D12,
        METAL
};

class LUDDITE_API Application
{
        public:
        Application();
        virtual ~Application();
        void Run();
        void CreateMainWindow(const std::string& Name, int width = 1080, int height = 720, int min_width = 320, int min_height = 240);
        std::shared_ptr<Window> GetMainWindow() {return m_pMainWindow;}

        protected:
        virtual void Initialize() {}
        virtual void OnUpdate(float delta_time) = 0;
        virtual void OnFixedUpdate(float delta_time) = 0;
        virtual void OnRender(float lerp_alpha) = 0;
        virtual void OnImGuiRender(float lerp_alpha) {};
        std::shared_ptr<Window> m_pMainWindow;
        RenderingBackend m_RenderingBackend;
};

extern Application* CreateApplication();
}
