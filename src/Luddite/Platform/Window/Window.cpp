#include "Luddite/Platform/Window/Window.hpp"

namespace Luddite
{
Window::Window()
{
}
Window::~Window() {}
void Window::OnWindowResize(int width, int height)
{
        m_WindowWidth = width;
        m_WindowHeight = height;
        Renderer::OnWindowResize(width, height);
        m_pSwapChain->Resize(width, height);
}

void Window::SwapBuffers()
{
        m_pSwapChain->Present(m_Vsync ? 1 : 0);
}

void Window::HandleEvents()
{
        {
        }
        unsigned int new_width = 0;
        unsigned int new_height = 0;
        for (auto& event : m_EventPool.GetList<WindowSizeEvent>())
        {
                new_width = event.width;
                new_height = event.height;
                event.SetHandled();
        }
        if (new_width != 0 && new_height != 0)
        {
                OnWindowResize(new_width, new_height);
        }
}

ImGuiContext* Window::GetImGuiContext()
{
        return ImGui::GetCurrentContext();
};

glm::mat4x4 Window::GetAdjustedProjectionMatrix(float FOV, float NearPlane, float FarPlane) const
{
        const auto& SCDesc = m_pSwapChain->GetDesc();

        float AspectRatio = static_cast<float>(SCDesc.Width) / static_cast<float>(SCDesc.Height);
        float XScale, YScale;
        if (SCDesc.PreTransform == Diligent::SURFACE_TRANSFORM_ROTATE_90 ||
            SCDesc.PreTransform == Diligent::SURFACE_TRANSFORM_ROTATE_270 ||
            SCDesc.PreTransform == Diligent::SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90 ||
            SCDesc.PreTransform == Diligent::SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270)
        {
                // When the screen is rotated, vertical FOV becomes horizontal FOV
                XScale = 1.f / std::tan(FOV / 2.f);
                // Aspect ratio is inversed
                YScale = XScale * AspectRatio;
        }
        else
        {
                YScale = 1.f / std::tan(FOV / 2.f);
                XScale = YScale / AspectRatio;
        }


        // Diligent::float4x4 Proj;
        // Proj[1][1] = XScale;
        // Proj[2][2] = YScale;
        // Proj.SetNearFarClipPlanes(NearPlane, FarPlane, false);//m_pDevice->GetDeviceCaps().IsGLDevice());
        glm::mat4x4 Proj = glm::perspective(FOV, YScale / XScale, NearPlane, FarPlane);
        return Proj;
}

glm::mat4x4 Window::GetSurfacePretransformMatrix(const glm::vec3& f3CameraViewAxis) const
{
        const auto& SCDesc = m_pSwapChain->GetDesc();
        switch (SCDesc.PreTransform)
        {
        case Diligent::SURFACE_TRANSFORM_ROTATE_90:
                // The image content is rotated 90 degrees clockwise.
                return glm::rotate(glm::half_pi<float>(), f3CameraViewAxis);

        case Diligent::SURFACE_TRANSFORM_ROTATE_180:
                // The image content is rotated 180 degrees clockwise.
                return glm::rotate(-glm::pi<float>(), f3CameraViewAxis);

        case Diligent::SURFACE_TRANSFORM_ROTATE_270:
                // The image content is rotated 270 degrees clockwise.
                return glm::rotate(-3 * glm::half_pi<float>(), f3CameraViewAxis);

        case Diligent::SURFACE_TRANSFORM_OPTIMAL:
                UNEXPECTED("SURFACE_TRANSFORM_OPTIMAL is only valid as parameter during swap chain initialization.");
                return glm::mat4(1.f);
        // return Diligent::float4x4::Identity();

        case Diligent::SURFACE_TRANSFORM_HORIZONTAL_MIRROR:
        case Diligent::SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90:
        case Diligent::SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180:
        case Diligent::SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270:
                UNEXPECTED("Mirror transforms are not supported");
                return glm::mat4x4(1.f);

        default:
                return glm::mat4x4(1.f);
        }
}
}