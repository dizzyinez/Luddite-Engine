#include "Luddite/Core/pch.hpp"
#include "btBulletDynamicsCommon.h"

struct C_PhysicsWorld
{
        // ~C_PhysicsWorld()
        // {
        //         //remove the rigidbodies from the dynamics world and delete them
        //         if (dynamics_world)
        //         {
        //                 int i;
        //                 for (i = dynamics_world->getNumConstraints() - 1; i >= 0; i--)
        //                 {
        //                         dynamics_world->removeConstraint(dynamics_world->getConstraint(i));
        //                 }
        //                 for (i = dynamics_world->getNumCollisionObjects() - 1; i >= 0; i--)
        //                 {
        //                         btCollisionObject* obj = dynamics_world->getCollisionObjectArray()[i];
        //                         btRigidBody* body = btRigidBody::upcast(obj);
        //                         if (body && body->getMotionState())
        //                         {
        //                                 delete body->getMotionState();
        //                         }
        //                         dynamics_world->removeCollisionObject(obj);
        //                         delete obj;
        //                 }
        //         }

        //         //delete collision shapes
        //         for (int j = 0; j < collision_shapes.size(); j++)
        //         {
        //                 btCollisionShape* shape = collision_shapes[j];
        //                 delete shape;
        //         }
        //         collision_shapes.clear();
        //         delete broadphase;
        //         delete dispatcher;
        //         delete solver;
        //         delete collision_configuration;
        // }
        // btAlignedObjectArray<btCollisionShape*> collision_shapes;
        btBroadphaseInterface* broadphase = nullptr;
        btCollisionDispatcher* dispatcher = nullptr;
        btConstraintSolver* solver = nullptr;
        btDefaultCollisionConfiguration* collision_configuration = nullptr;
        btDiscreteDynamicsWorld* dynamics_world = nullptr;
};