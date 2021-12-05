#pragma once
#include "Luddite/Layers/Editor/Panel.hpp"
namespace Luddite
{
class LUDDITE_API HeirarchyPanel : public Panel
{
        virtual void OnDraw(EditorContext& ctx) override;
        virtual const char* GetName() override {return "Heirarchy";}
};
}
