#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/ECS/Reflection.hpp"
#include "Luddite/ECS/Modules/LudditeBase.hpp"

namespace Transform3D
{
LD_COMPONENT_DEFINE(Translation,
        (
                inline glm::mat4 GetMatrix() const {return glm::translate(glm::mat4(1.f), Translation);}
        ),
        ((glm::vec3)Translation, ({0.f, 0.f, 0.f}))
        )

LD_COMPONENT_DEFINE(LocalTranslation,
        (),
        ((glm::vec3)Translation, ({0.f, 0.f, 0.f}))
        )

LD_COMPONENT_DEFINE(Rotation,
        (
                inline glm::mat4 GetMatrix() const {return glm::toMat4(glm::quat(Rotation));}
                inline glm::vec3 GetLookDirection() const {return glm::toMat4(glm::quat(Rotation)) * glm::vec4(0.f, 0.f, 1.f, 0.f);}
        ),
        ((glm::vec3)Rotation, ({0.f, 0.f, 0.f}))
        )

LD_COMPONENT_DEFINE(LocalRotation,
        (),
        ((glm::vec3)Rotation, ({0.f, 0.f, 0.f}))
        )

LD_COMPONENT_DEFINE(Scale,
        (
                inline glm::mat4 GetMatrix() const {return glm::scale(glm::mat4(1.f), Scale);}
        ),
        ((glm::vec3)Scale, ({1.f, 1.f, 1.f}))
        )

LD_COMPONENT_DEFINE(LocalScale,
        (),
        ((glm::vec3)Scale, ({1.f, 1.f, 1.f}))
        )

struct Components
{
        Components(flecs::world& w)
        {
                w.module<Components>();
                //w.component<Translation>();
                //w.component<Translation>("Translation");
                //w.component<Scale>("Scale");
                //w.component<Rotation>("Rotation");
                LD_COMPONENT_REGISTER(Translation, w);
                LD_COMPONENT_REGISTER(LocalTranslation, w);
                LD_COMPONENT_REGISTER(Rotation, w);
                LD_COMPONENT_REGISTER(LocalRotation, w);
                LD_COMPONENT_REGISTER(Scale, w);
                LD_COMPONENT_REGISTER(LocalScale, w);

                //w.id<Luddite::Scene>().entity().add<Translation>();
                //w.id<Luddite::Scene>().entity().add<Rotation>();
        }
};

struct Systems
{
        Systems(flecs::world& w)
        {
                w.import<Luddite::Components>();
                w.import<Components>();
                w.module<Systems>();
                w.system<const LocalTranslation, const Translation, const Rotation, const Scale, Translation>("Apply Parent Translation")
                .arg(2).set(flecs::Parent, flecs::Cascade).oper(flecs::Optional)
                .arg(3).set(flecs::Parent, flecs::Cascade).oper(flecs::Optional)
                .arg(4).set(flecs::Parent, flecs::Cascade).oper(flecs::Optional)
                .kind(w.id<Luddite::PostSimulate>())
                .iter([](flecs::iter it, const LocalTranslation* lt, const Translation* t_p, const Rotation* r_p, const Scale* s_p, Translation* t){
                                for (auto i : it)
                                {
                                        glm::mat4 parent_transform{1};
                                        if (t_p)
                                                parent_transform *= t_p[i].GetMatrix();
                                        if (r_p)
                                                parent_transform *= r_p[i].GetMatrix();
                                        if (s_p)
                                                parent_transform *= s_p[i].GetMatrix();

                                        t[i].Translation = glm::vec3(parent_transform * glm::vec4(lt->Translation, 1.f));
                                }
                        });
                w.system<const LocalRotation, const Rotation, Rotation>("Apply Parent Rotation")
                .arg(2).set(flecs::Parent, flecs::Cascade).oper(flecs::Optional)
                .kind(w.id<Luddite::PostSimulate>())
                .iter([](flecs::iter it, const LocalRotation* lr, const Rotation* r_p, Rotation* r){
                                for (auto i : it)
                                {
                                        r[i].Rotation = lr[i].Rotation;
                                        if (r_p)
                                                r[i].Rotation += r_p->Rotation;
                                }
                        });
                w.system<const LocalScale, const Scale, Scale>("Apply Parent Scale")
                .arg(2).set(flecs::Parent, flecs::Cascade).oper(flecs::Optional)
                .kind(w.id<Luddite::PostSimulate>())
                .iter([](flecs::iter it, const LocalScale* ls, const Scale* s_p, Scale* s){
                                for (auto i : it)
                                {
                                        s[i].Scale = ls[i].Scale;
                                        if (s_p)
                                                s[i].Scale += s_p->Scale;
                                }
                        });
        }
};
}
