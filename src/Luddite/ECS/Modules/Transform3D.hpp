#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/ECS/Reflection.hpp"
#include "Luddite/ECS/Modules/LudditeBase.hpp"
#include "glm/gtx/matrix_decompose.hpp"

namespace Transform3D
{
LD_COMPONENT_DEFINE(Translation,
        (
                inline glm::mat4 GetMatrix() const {return glm::translate(glm::mat4(1.f), Translation);}
        ),
        ((glm::vec3)Translation, ({0.f, 0.f, 0.f}))
        )

LD_COMPONENT_DEFINE(LocalTranslation,
        (
                inline glm::mat4 GetMatrix() const {return glm::translate(glm::mat4(1.f), Translation);}
        ),
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
        (
                inline glm::mat4 GetMatrix() const {return glm::toMat4(glm::quat(Rotation));}
                inline glm::vec3 GetLookDirection() const {return glm::toMat4(glm::quat(Rotation)) * glm::vec4(0.f, 0.f, 1.f, 0.f);}
        ),
        ((glm::vec3)Rotation, ({0.f, 0.f, 0.f}))
        )

LD_COMPONENT_DEFINE(Scale,
        (
                inline glm::mat4 GetMatrix() const {return glm::scale(glm::mat4(1.f), Scale);}
        ),
        ((glm::vec3)Scale, ({1.f, 1.f, 1.f}))
        )

LD_COMPONENT_DEFINE(LocalScale,
        (
                inline glm::mat4 GetMatrix() const {return glm::scale(glm::mat4(1.f), Scale);}
        ),
        ((glm::vec3)Scale, ({1.f, 1.f, 1.f}))
        )

struct SetLocalTransform
{
        glm::vec3 Translation;
        glm::vec3 Rotation;
        glm::vec3 Scale;
};

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

                w.observer<LocalTranslation, LocalRotation, LocalScale>().event<SetLocalTransform>()
                .iter([](flecs::iter it, LocalTranslation* lt, LocalRotation* lr, LocalScale* ls){
                                auto* set = it.param<SetLocalTransform>();
                                for (auto i : it)
                                {
                                        lt[i].Translation = set[i].Translation;
                                        lr[i].Rotation = set[i].Rotation;
                                        ls[i].Scale = set[i].Scale;
                                }
                        });


                //w.system<const LocalTranslation, const LocalRotation, const LocalScale, const Translation, const Rotation, const Scale, Translation, Rotation, Scale>("Apply parent transform")
                //.arg(4).set(flecs::Parent, flecs::Cascade).oper(flecs::Optional)
                //.arg(5).set(flecs::Parent, flecs::Cascade).oper(flecs::Optional)
                //.arg(6).set(flecs::Parent, flecs::Cascade).oper(flecs::Optional)
                //.kind(w.id<Luddite::PostSimulate>())
                //.iter([](flecs::iter it, const LocalTranslation* lt, const LocalRotation* lr, const LocalScale* ls, const Translation* pt, const Rotation* pr, const Scale* ps, Translation* t, Rotation* r, Scale* s){
                //                for (auto i : it)
                //                {
                //                        const glm::mat4 mat_local = lt[i].GetMatrix() * lr[i].GetMatrix() * ls[i].GetMatrix();
                //                        glm::mat4 mat_parent = glm::identity<glm::mat4>();
                //                        if (pt)
                //                                mat_parent *= pt[i].GetMatrix();
                //                        if (pr)
                //                                mat_parent *= pr[i].GetMatrix();
                //                        if (ps)
                //                                mat_parent *= ps[i].GetMatrix();
                //                        const glm::mat4 mat_global = mat_parent * mat_local;
                //                        //const glm::mat4 mat_global = mat_local * mat_parent;

                //                        glm::vec3 scale;
                //                        glm::quat rotation;
                //                        glm::vec3 translation;
                //                        glm::vec3 skew;
                //                        glm::vec4 perspective;
                //                        glm::decompose(mat_global, scale, rotation, translation, skew, perspective);

                //                        t[i].Translation = translation;
                //                        r[i].Rotation = glm::eulerAngles(rotation);
                //                        s[i].Scale = scale;
                //                }
                //        });


                w.system<const LocalTranslation, const Translation, const Rotation, const Scale, Translation>("Apply Parent Translation")
                .arg(2).set(flecs::Parent | flecs::Cascade).oper(flecs::Optional)
                .arg(3).set(flecs::Parent).oper(flecs::Optional)
                .arg(4).set(flecs::Parent).oper(flecs::Optional)
                .kind(w.id<Luddite::PostSimulate>())
                .iter([](flecs::iter it, const LocalTranslation* lt, const Translation* t_p, const Rotation* r_p, const Scale* s_p, Translation* t){
                                for (auto i : it)
                                {
                                        glm::mat4 parent_transform = glm::identity<glm::mat4>();
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
                .arg(2).set(flecs::Parent | flecs::Cascade).oper(flecs::Optional)
                .kind(w.id<Luddite::PostSimulate>())
                .iter([](flecs::iter it, const LocalRotation* lr, const Rotation* r_p, Rotation* r){
                                for (auto i : it)
                                {
                                        r[i].Rotation = lr[i].Rotation;
                                        if (r_p)
                                        {
                                                r[i].Rotation = glm::eulerAngles(glm::quat(r_p[i].Rotation) * glm::quat(lr[i].Rotation));
                                                //r[i].Rotation += r_p[i].Rotation;
                                        }
                                }
                        });

                w.system<const LocalScale, const Scale, Scale>("Apply Parent Scale")
                .arg(2).set(flecs::Parent | flecs::Cascade).oper(flecs::Optional)
                .kind(w.id<Luddite::PostSimulate>())
                .iter([](flecs::iter it, const LocalScale* ls, const Scale* s_p, Scale* s){
                                for (auto i : it)
                                {
                                        s[i].Scale = ls[i].Scale;
                                        if (s_p)
                                        {
                                                s[i].Scale *= s_p->Scale;
                                        }
                                }
                        });
        }
};
}
