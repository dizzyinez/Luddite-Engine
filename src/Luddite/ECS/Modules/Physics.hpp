#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/ECS/Modules/LudditeBase.hpp"
#include "Luddite/ECS/Modules/Transform3D.hpp"
#include "Luddite/ECS/Reflection.hpp"

namespace Physics {
LD_COMPONENT_DEFINE(LinearVelocity,
        (),
        ((glm::vec3)Velocity, ({0.f, 0.f, 0.f}))
        )

LD_COMPONENT_DEFINE(AngularVelocity,
        (),
        ((glm::vec3)Velocity, ({0.f, 0.f, 0.f}))
        )

LD_COMPONENT_DEFINE(LinearDamping,
        (),
        ((glm::vec3)Damping, ({0.f, 0.f, 0.f}))
        )

LD_COMPONENT_DEFINE(AngularDamping,
        (),
        ((glm::vec3)Damping, ({0.f, 0.f, 0.f}))
        )

LD_COMPONENT_DEFINE(CircleCollider,
        (),
        ((float)radius, (1.f), .min = 0.f, .flags = ImGuiSliderFlags_ClampOnInput | ImGuiSliderFlags_ClampOnInput)
        )

struct Components
{
        Components(flecs::world &w) {
                w.module<Components>();
                LD_COMPONENT_REGISTER(LinearVelocity, "Linear Velocity", w);
                LD_COMPONENT_REGISTER(AngularVelocity, "Angular Velocity", w);
                LD_COMPONENT_REGISTER(LinearDamping, "Linear Damping", w);
                LD_COMPONENT_REGISTER(AngularDamping, "Angular Damping", w);
                LD_COMPONENT_REGISTER(CircleCollider, "Circle Collider", w);
        }
};

struct Systems
{
        Systems(flecs::world &w)
        {
                w.import<Luddite::Components>();
                w.import<Transform3D::Components>();
                w.import<Components>();
                w.module<Systems>();
                w.import<Transform3D::Components>();
                w.import<Components>();
                w.module<Systems>();

                //Motion
                w.system<const LinearVelocity, Transform3D::Translation>("Add Linear Velocity")
                .kind(w.id<Luddite::OnSimulate>())
                .iter([](flecs::iter it){
                                auto v = it.term<const LinearVelocity>(1);
                                auto t = it.term<Transform3D::Translation>(2);
                                for (auto i : it)
                                        t[i].Translation += v[i].Velocity * it.delta_time();
                        });

                w.system<const AngularVelocity, Transform3D::Rotation>("Add Angular Velocity")
                .kind(w.id<Luddite::OnSimulate>())
                .iter([](flecs::iter it){
                                auto v = it.term<const AngularVelocity>(1);
                                auto r = it.term<Transform3D::Rotation>(2);
                                for (auto i : it)
                                        r[i].Rotation += v[i].Velocity * it.delta_time();
                        });

                w.system<const LinearDamping, LinearVelocity>("Linear Damping")
                .kind(w.id<Luddite::PostSimulate>())
                .each([](flecs::entity e, const LinearDamping& d, LinearVelocity& v){
                                v.Velocity *= glm::pow(d.Damping, glm::vec3(e.delta_time()));
                        });

                w.system<const AngularDamping, AngularVelocity>("Angular Damping")
                .kind(w.id<Luddite::PostSimulate>())
                .each([](flecs::entity e, const AngularDamping& d, AngularVelocity& v){
                                v.Velocity *= glm::pow(d.Damping, glm::vec3(e.delta_time()));
                        });

                //Broad Phase
        }
};
} // namespace Physics
