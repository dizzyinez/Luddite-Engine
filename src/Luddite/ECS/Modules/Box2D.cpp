#include "Luddite/ECS/Modules/Box2D.hpp"
#include "Luddite/ECS/Modules/Physics.hpp"
#include "box2d/box2d.h"

namespace Box2D
{
Components::Components(flecs::world& w)
{
        w.module<Components>();
        w.component<PhysicsWorld>();
        w.component<RigidBody>();
        LD_COMPONENT_REGISTER(BoxCollider, w);
        LD_COMPONENT_REGISTER(PhysicsMaterial, w);
}

Systems::Systems(flecs::world& w)
{
        w.import<Luddite::Components>();
        w.import<Transform3D::Components>();
        w.import<Physics::Components>();
        w.import<Components>();
        w.module<Systems>();
        w.set<PhysicsWorld>({new b2World({0, 0})});
        w.trigger<PhysicsWorld>("Cleanup Physics World").event(flecs::OnRemove).each([](flecs::entity e, PhysicsWorld& p){
                        if (p.m_pWorld)
                                delete p.m_pWorld;
                });

        w.observer<RigidBody, const Transform3D::Translation, const Transform3D::Rotation>("Setup RigidBody").event(flecs::OnSet)
        .iter([](flecs::iter it){
                        auto rb = it.term<RigidBody>(1);
                        auto t = it.term<const Transform3D::Translation>(2);
                        auto r = it.term<const Transform3D::Rotation>(3);
                        //auto pw = it.term<PhysicsWorld>(4);
                        auto pw = it.world().get<PhysicsWorld>();
                        for (auto i : it)
                        {
                                if (rb[i].m_pRuntimeBody)
                                {
                                        rb[i].m_pRuntimeBody->SetTransform({t[i].Translation.x, t[i].Translation.y}, r[i].Rotation.z);
                                        rb[i].m_pRuntimeBody->SetType((b2BodyType)rb[i].Type);
                                        rb[i].m_pRuntimeBody->SetFixedRotation(rb[i].FixRotation);
                                }
                                else
                                {
                                        b2BodyDef def;
                                        def.type = (b2BodyType)rb[i].Type;
                                        def.position.Set(t[i].Translation.x, t[i].Translation.y);
                                        def.angle = r[i].Rotation.z;
                                        def.fixedRotation = rb[i].FixRotation;

                                        b2Body* body = pw->m_pWorld->CreateBody(&def);
                                        rb[i].m_pRuntimeBody = body;
                                }
                        }
                });

        w.system<RigidBody, const Physics::LinearVelocity>("Set RigidBody Linear Velocity")
        .kind(w.id<Luddite::OnSimulate>())
        .iter([](flecs::iter it){
                        auto rb = it.term<RigidBody>(1);
                        auto v = it.term<const Physics::LinearVelocity>(2);
                        for (auto i : it)
                        {
                                rb[i].m_pRuntimeBody->SetLinearVelocity({v[i].Velocity.x, v[i].Velocity.y});
                        }
                });

        w.system<RigidBody, const Physics::AngularVelocity>("Set RigidBody Angular Velocity")
        .kind(w.id<Luddite::OnSimulate>())
        .iter([](flecs::iter it){
                        auto rb = it.term<RigidBody>(1);
                        auto v = it.term<const Physics::AngularVelocity>(2);
                        for (auto i : it)
                        {
                                rb[i].m_pRuntimeBody->SetAngularVelocity(v->Velocity.z);
                        }
                });

        w.observer<RigidBody, const Physics::LinearDamping>("Set RigidBody Linear Damping").event(flecs::OnSet)
        .iter([](flecs::iter it){
                        auto rb = it.term<RigidBody>(1);
                        auto d = it.term<const Physics::LinearDamping>(2);
                        for (auto i : it)
                        {
                                rb[i].m_pRuntimeBody->SetLinearDamping(d->Damping.x);
                        }
                });

        w.observer<RigidBody, const Physics::AngularDamping>("Set RigidBody Angular Damping").event(flecs::OnSet)
        .iter([](flecs::iter it){
                        auto rb = it.term<RigidBody>(1);
                        auto d = it.term<const Physics::AngularDamping>(2);
                        for (auto i : it)
                        {
                                rb[i].m_pRuntimeBody->SetAngularDamping(d->Damping.z);
                        }
                });

        w.observer<const RigidBody, const PhysicsMaterial, const BoxCollider>("Setup Box Collider").event(flecs::OnSet)
        .iter([](flecs::iter it){
                        auto rb = it.term<const RigidBody>(1);
                        auto pm = it.term<const PhysicsMaterial>(2);
                        auto bc = it.term<const BoxCollider>(3);
                        for (auto i : it)
                        {
                                b2PolygonShape box;
                                box.SetAsBox(bc[i].Size.x, bc[i].Size.y);

                                b2FixtureDef fixture;
                                fixture.shape = &box;
                                fixture.density = pm[i].Density;
                                fixture.friction = pm[i].Friction;
                                fixture.restitution = pm[i].Restitution;
                                fixture.restitutionThreshold = pm[i].RestitutionThreshold;

                                rb[i].m_pRuntimeBody->CreateFixture(&fixture);
                        }
                });

        w.system<>("Step Box2D")
        .term<const PhysicsWorld>().singleton()
        .kind(w.id<Luddite::OnSimulate>())
        .iter([](flecs::iter it){
                        auto pw = it.term<const PhysicsWorld>(1);
                        pw->m_pWorld->Step(it.delta_time(), pw->VelocityIterations, pw->PositionIterations);
                });

        w.system<const RigidBody, Transform3D::Translation, Transform3D::Rotation>("Update Translation and Rotation from RigidBodies")
        .kind(w.id<Luddite::OnSimulate>())
        .iter([](flecs::iter it){
                        auto rb = it.term<const RigidBody>(1);
                        auto t = it.term<Transform3D::Translation>(2);
                        auto r = it.term<Transform3D::Rotation>(3);
                        for (auto i : it)
                        {
                                const auto& pos = rb[i].m_pRuntimeBody->GetPosition();
                                if (pos.x != t[i].Translation.x || pos.y != t[i].Translation.y)
                                {
                                        t[i].Translation.x = pos.x;
                                        t[i].Translation.y = pos.y;
                                        it.entity(i).modified<Transform3D::Translation>();
                                }
                                const auto& angle = rb[i].m_pRuntimeBody->GetAngle();
                                if (angle != r[i].Rotation.z)
                                {
                                        r[i].Rotation.z = angle;
                                        it.entity(i).modified<Transform3D::Rotation>();
                                }
                        }
                });

        w.system<const RigidBody, Physics::LinearVelocity>("Update Linear Velocity from RigidBodies")
        .kind(w.id<Luddite::OnSimulate>())
        .iter([](flecs::iter it){
                        auto rb = it.term<const RigidBody>(1);
                        auto v = it.term<Physics::LinearVelocity>(2);
                        for (auto i : it)
                        {
                                const auto& vel = rb[i].m_pRuntimeBody->GetLinearVelocity();
                                if (vel.x != v[i].Velocity.x || vel.y != v[i].Velocity.y)
                                {
                                        v[i].Velocity.x = vel.x;
                                        v[i].Velocity.y = vel.y;
                                        it.entity(i).modified<Physics::LinearVelocity>();
                                }
                        }
                });

        w.system<const RigidBody, Physics::AngularVelocity>("Update Angular Velocity from RigidBodies")
        .kind(w.id<Luddite::OnSimulate>())
        .iter([](flecs::iter it){
                        auto rb = it.term<const RigidBody>(1);
                        auto v = it.term<Physics::AngularVelocity>(2);
                        for (auto i : it)
                        {
                                const auto& vel = rb[i].m_pRuntimeBody->GetAngularVelocity();
                                if (v[i].Velocity.z != vel)
                                {
                                        v[i].Velocity.z = rb[i].m_pRuntimeBody->GetAngularVelocity();
                                        it.entity(i).modified<Physics::AngularVelocity>();
                                }
                        }
                });
}
}
