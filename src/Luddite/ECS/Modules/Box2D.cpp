#include "Luddite/ECS/Modules/Box2D.hpp"
#include "box2d/box2d.h"
namespace Box2D
{
Components::Components(flecs::world& w)
{
        w.module<Components>();
        w.component<PhysicsWorld>("Physics World");
        w.component<RigidBody>();
        LD_COMPONENT_REGISTER(BoxCollider, "Box Collider", w);
        LD_COMPONENT_REGISTER(PhysicsMaterial, "Physics Material", w);
}

Systems::Systems(flecs::world& w)
{
        w.import<Luddite::Components>();
        w.import<Transform3D::Components>();
        w.import<Components>();
        w.module<Systems>();
        w.set<PhysicsWorld>({new b2World({0, -9.8})});
        w.trigger<PhysicsWorld>("Cleanup Physics World").event(flecs::OnRemove).each([](flecs::entity e, PhysicsWorld& p){
                        if (p.m_pWorld)
                                delete p.m_pWorld;
                });
        w.observer<RigidBody, const Transform3D::Translation, const Transform3D::Rotation>("Setup RigidBody").event(flecs::OnSet)
        //w.observer<RigidBody>("Setup RigidBody").event(flecs::OnSet)
        //.term<const Transform3D::Translation>()
        //.term<const Transform3D::Rotation>()
        //.term<PhysicsWorld>().singleton().inout(flecs::InOut)
        .iter([](flecs::iter it){
                        auto rb = it.term<RigidBody>(1);
                        auto t = it.term<const Transform3D::Translation>(2);
                        auto r = it.term<const Transform3D::Rotation>(3);
                        //auto pw = it.term<PhysicsWorld>(4);
                        auto pw = it.world().get<PhysicsWorld>();
                        for (auto i : it)
                        {
                                b2BodyDef def;
                                def.type = (b2BodyType)rb[i].Type;
                                def.position.Set(t[i].Translation.x, t[i].Translation.y);
                                def.angle = r[i].Rotation.z;

                                b2Body* body = pw->m_pWorld->CreateBody(&def);
                                body->SetFixedRotation(rb[i].FixRotation);
                                rb[i].m_pRuntimeBody = body;
                                LD_VERIFY(body != nullptr, "Failed to create b2d rigid body!");
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
                                LD_LOG_INFO("test");
                        }
                });

        //w.system<>()
        //.term<const PhysicsWorld>().singleton()
        //.kind(w.id<Luddite::OnSimulate>())
        //.iter([](flecs::iter it){
        //                auto pw = it.term<const PhysicsWorld>(1);
        //                pw->m_pWorld->Step(it.delta_time(), pw->VelocityIterations, pw->PositionIterations);
        //        });

        //w.system<RigidBody, Transform3D::Translation, Transform3D::Rotation>()
        //.kind(w.id<Luddite::OnSimulate>())
        //.iter([](flecs::iter it){
        //                auto rb = it.term<RigidBody>(1);
        //                auto t = it.term<Transform3D::Translation>(2);
        //                auto r = it.term<Transform3D::Rotation>(3);
        //                for (auto i : it)
        //                {
        //                        const auto& pos = rb[i].m_pRuntimeBody->GetPosition();
        //                        t[i].Translation.x = pos.x;
        //                        t[i].Translation.y = pos.y;
        //                        r[i].Rotation.z = rb[i].m_pRuntimeBody->GetAngle();
        //                }
        //        });
}
}
