#pragma once
#include "Luddite/Layers/Editor/Panel.hpp"
namespace Luddite
{
class LUDDITE_API ViewportPanel : public Panel
{
        private:
        Luddite::RenderTexture m_RenderTexture;
        glm::ivec2 size = glm::ivec2(500, 500);
        //float m_CamPitch = 0.f;
        //float m_CamYaw = 0.f;
        //float m_CamZoom = 10.f;
        void Resize();
        bool SetRenderTexture();
        void CheckForResize();
        public:
        ViewportPanel();
        virtual void OnRender(EditorContext& ctx) override;
        virtual void OnDraw(EditorContext& ctx) override;
        virtual const char* GetName() override {return "Viewport";}
};
}
