#include "Luddite/Layers/Editor/HeirarchyPanel.hpp"
#include "Luddite/Layers/EditorLayer.hpp"
#include "Luddite/ECS/Modules/LudditeBase.hpp"
#include "imgui.h"

void DrawEntityTree(Luddite::EditorContext& ctx, flecs::entity e)
{
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
        //if (ctx.selection.size() > 0 && e == ctx.selection.back())
        if (std::find(ctx.selection.begin(), ctx.selection.end(), e) != ctx.selection.end())
                flags |= ImGuiTreeNodeFlags_Selected;
        bool opened = ImGui::TreeNodeEx((void*)e.id(), flags, e.name().c_str());
        if (ImGui::IsItemClicked())
        {
                ctx.selection = {e};
        }
        if (opened)
        {
                e.children([&](flecs::entity child) {
                        DrawEntityTree(ctx, child);
                });
                ImGui::TreePop();
        }
}

void Luddite::HeirarchyPanel::OnDraw(EditorContext& ctx)
{
        if (!open)
                return;
        if (ImGui::Begin("Heirarchy", &open, ImGuiWindowFlags_NoCollapse /*| ImGuiWindowFlags_MenuBar*/))
        {
                DrawEntityTree(ctx, ctx.world->id<Luddite::Scene>().entity());
        }
        ImGui::End();
}
