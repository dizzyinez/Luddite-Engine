#pragma once
#include "Luddite/pch.hpp"
#include "Luddite/Core.hpp"
#include "Luddite/LayerStack.hpp"
#include "Luddite/Graphics/Renderer.hpp"
#include "Luddite/Graphics/RenderTarget.hpp"

namespace Luddite
{
class LUDDITE_API Window
{
public:
        Window();
        virtual ~Window();
        virtual void SetTitle(const std::string& title) {};
        virtual void Resize(uint32_t width, uint32_t height) {};
        virtual uint32_t GetWidth() = 0;
        virtual uint32_t GetHeight() = 0;
        LayerStack& GetLayerStack() {return m_layer_stack;}
        virtual void HandleEvents() = 0;
        inline bool ShouldQuit() const {return Quit;}
        void SwapBuffers();

        inline RenderTarget GetRenderTarget() {
                auto& SCDesc = m_pSwapChain->GetDesc();
                return {
                        m_pSwapChain->GetCurrentBackBufferRTV(),
                        m_pSwapChain->GetDepthBufferDSV(),
                        SCDesc.Width,
                        SCDesc.Height,
                        true
                };
        }

        glm::mat4x4 GetAdjustedProjectionMatrix(float FOV, float NearPlane, float FarPlane) const;
        glm::mat4x4 GetSurfacePretransformMatrix(const glm::vec3& f3CameraViewAxis) const;



        #ifdef LD_ENABLE_IMGUI
        inline std::unique_ptr<Diligent::ImGuiImplDiligent>& GetImGuiImpl() {return m_pImGuiImpl;}
        inline void ImGuiNewFrame()
        {
                m_pImGuiImpl->NewFrame(GetWidth(), GetHeight(), m_pSwapChain->GetDesc().PreTransform);
        };
        #else
        inline void ImGuiNewFrame() {};
        #endif // LD_ENABLE_IMGUI
protected:
        void OnWindowResize(int width, int height);

        Diligent::RefCntAutoPtr<Diligent::ISwapChain> m_pSwapChain;
        bool Quit = false;

        #ifdef LD_ENABLE_IMGUI
        std::unique_ptr<Diligent::ImGuiImplDiligent>  m_pImGuiImpl;
        #endif // LD_ENABLE_IMGUI

        LayerStack m_layer_stack;
};
}