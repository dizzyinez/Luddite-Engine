#include "Luddite/Core/pch.hpp"
#include "btBulletDynamicsCommon.h"

struct C_RigidBody
{
        C_RigidBody(float mass_ = 1.f) : mass(mass_) {}
        float mass = 1.0f;
        btRigidBody* rigid_body = nullptr;
};