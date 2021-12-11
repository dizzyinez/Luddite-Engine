#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/ECS/Modules/LudditeBase.hpp"
#include "Luddite/ECS/Modules/Transform3D.hpp"
#include "Luddite/ECS/Reflection.hpp"

struct b2World;
struct b2Body;
namespace Box2D
{
struct PhysicsWorld
{
        b2World* m_pWorld = nullptr;
        int VelocityIterations = 6;
        int PositionIterations = 2;
};
struct RigidBody
{
        enum class BodyType { Static = 0, Kinematic, Dynamic };
        BodyType Type = BodyType::Static;
        b2Body* m_pRuntimeBody;
        bool FixRotation;
};
struct Components
{
        Components(flecs::world& w);
};
struct Systems
{
        Systems(flecs::world& w);
};
LD_COMPONENT_DEFINE(BoxCollider,
        (),
        ((glm::vec2)Size, ({1, 1}))
        )
LD_COMPONENT_DEFINE(PhysicsMaterial,
        (),
        ((float)Density, (1.f)),
        ((float)Friction, (0.5f)),
        ((float)Restitution, (0.f)),
        ((float)RestitutionThreshold, (0.5f))
        )
}
