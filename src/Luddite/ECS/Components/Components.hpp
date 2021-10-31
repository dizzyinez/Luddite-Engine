#pragma once
#include "Luddite/ECS/Components/Transform.hpp"
#include "Luddite/ECS/Components/Model.hpp"
#include "Luddite/ECS/Components/Camera.hpp"
#include "Luddite/ECS/Components/Lights.hpp"
#include "Luddite/ECS/Components/Name.hpp"
#include "Luddite/ECS/Components/RigidBody.hpp"
#include "Luddite/ECS/Components/CollisionShape.hpp"
#include "Luddite/ECS/Components/Collider.hpp"
#include "Luddite/ECS/Components/NativeScript.hpp"
#define LD_COMPONENTS C_Transform3D, C_Model, C_Camera, C_PointLight, C_SpotLight, C_DirectionalLight, C_Name, C_RigidBody, C_CollisionShape, C_Collider, C_NativeScript

//Singletons
#include "Luddite/ECS/Components/ActiveCamera.hpp"
#include "Luddite/ECS/Components/PhysicsWorld.hpp"
#define LD_SINGLETONS C_ActiveCamera, C_PhysicsWorld

template <typename Component>
constexpr void RegisterTypeIds()
{
        auto v = entt::type_seq<Component>::value();
        LD_LOG_INFO("{}", v);
}
template <typename Component, typename Component2, typename ... Others>
constexpr void RegisterTypeIds()
{
        RegisterTypeIds<Component>();
        RegisterTypeIds<Component2, Others ...>();
}
constexpr void RegisterECSTypeIds()
{
        RegisterTypeIds<LD_COMPONENTS>();
        RegisterTypeIds<LD_SINGLETONS>();
}
// entt::type_seq<C_Transform3D>::value();
