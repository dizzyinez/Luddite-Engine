#pragma once
#include "Luddite/Layers/Editor/Panel.hpp"
namespace Luddite
{
class LUDDITE_API ComponentsPanel : public Panel
{
        virtual void OnDraw(EditorContext& ctx) override;
        virtual const char* GetName() override {return "Components";}
};
}
