#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/Graphics/RenderTarget.hpp"

namespace Luddite
{
struct EditorContext;
class LUDDITE_API Panel
{
        public:
        bool open = true;
        virtual void HandleEvents(EditorContext& ctx) {}
        virtual void Update(EditorContext& ctx) {}
        virtual void FixedUpdate(EditorContext& ctx) {}
        virtual void OnRender(EditorContext& ctx) {}
        virtual void OnDraw(EditorContext& ctx) {}
        virtual const char* GetName() = 0;
};
}
