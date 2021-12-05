#include "Luddite/Layers/Editor/ComponentsPanel.hpp"
#include "Luddite/ECS/Modules/LudditeBase.hpp"
#include "Luddite/Layers/EditorLayer.hpp"

void Luddite::ComponentsPanel::OnDraw(EditorContext& ctx)
{
        if (!open)
                return;
        if (ImGui::Begin("Components", &open))
        {
                if (ctx.selection.size() > 0)
                {
                        //https://github.com/SanderMertens/flecs/blob/master/examples/cpp/entities/iterate_components/src/main.cpp
                        auto& e = ctx.selection.back();
                        e.each([&](flecs::id id){
                                flecs::id role = id.role();
                                if (role)
                                {
                                        ImGui::Text("Role: %s", role.role_str().c_str());
                                }

                                if (id.is_pair())
                                {
                                        flecs::entity rel = id.relation();
                                        flecs::entity obj = id.object();
                                        ImGui::Text("Rel: %s, Obj: %s", rel.name().c_str(), obj.name().c_str());
                                }
                                else if (id.is_switch())
                                {}
                                else if (id.is_case())
                                {}
                                else
                                {
                                        flecs::entity comp = id.entity();
                                        ImGui::Text("Comp: %s", comp.name().c_str());
                                        auto* meta = comp.get<Luddite::ReflectionData>();
                                        if (comp.has<Luddite::ReflectionData>())
                                        {
                                                meta->ImGuiDraw(e.get_mut(comp));
                                        }
                                }
                                ImGui::Separator();
                        });
                }
        }
        ImGui::End();
}
