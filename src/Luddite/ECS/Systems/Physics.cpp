#include "Luddite/ECS/Systems/Physics.hpp"
#include "Luddite/ECS/World.hpp"
#include "Luddite/ECS/Components/Components.hpp"
#include "btBulletDynamicsCommon.h"
#include "Luddite/Core/Profiler.hpp"

ATTRIBUTE_ALIGNED16(struct)
MotionState : public btMotionState
{
        Luddite::Entity m_Entity;
        BT_DECLARE_ALIGNED_ALLOCATOR();

        MotionState(const Luddite::Entity& Entity_) : m_Entity(Entity_)
        {
        }

        virtual void getWorldTransform(btTransform& centerOfMassWorldTrans) const
        {
                const auto& c_transform = m_Entity.GetComponent<C_Transform3D>();
                // c_transform.Rotation
                centerOfMassWorldTrans =
                        btTransform(
                                btQuaternion(c_transform.Rotation.y, c_transform.Rotation.x, c_transform.Rotation.z),
                                btVector3(c_transform.Translation.x, c_transform.Translation.y, c_transform.Translation.z)
                                );
        }
        virtual void setWorldTransform(const btTransform& centerOfMassWorldTrans)
        {
                auto& c_transform = m_Entity.GetComponent<C_Transform3D>();
                btVector3 translate = centerOfMassWorldTrans.getOrigin();
                btQuaternion rotate = centerOfMassWorldTrans.getRotation();
                m_Entity.ReplaceComponent<C_Transform3D>(glm::vec3(translate.x(), translate.y(), translate.z()), glm::eulerAngles(glm::quat(rotate.w(), rotate.x(), rotate.y(), rotate.z())), c_transform.Scale);
        }
};

void on_construct_collision_shape(entt::registry& reg, entt::entity entity)
{
        C_CollisionShape& c_collision_shape = reg.get<C_CollisionShape>(entity);
        C_PhysicsWorld& c_physics_world = reg.ctx<C_PhysicsWorld>();

        int num_shapes = c_collision_shape.shapes.size();
        if (num_shapes > 0)
        {
                bool all_spheres = true;
                for (auto& shape : c_collision_shape.shapes)
                {
                        if (shape.type != C_CollisionShape::Shape::Sphere)
                                all_spheres = false;
                }
                if (all_spheres && num_shapes > 1)
                {
                        btVector3 positions[num_shapes];
                        btScalar radii[num_shapes];
                        for (int i = 0; i < num_shapes; i++)
                        {
                                glm::vec3& pos = c_collision_shape.shapes[i].offset;
                                positions[i] = btVector3(pos.x, pos.y, pos.z);
                                radii[i] = c_collision_shape.shapes[i].data.x;
                        }
                        c_collision_shape.shape = new btMultiSphereShape(positions, radii, num_shapes);
                        return;
                }
                else
                {
                        for (auto& shape : c_collision_shape.shapes)
                        {
                                switch (shape.type)
                                {
                                case C_CollisionShape::Shape::Box:
                                        shape.shape = new btBoxShape(btVector3(shape.data.x, shape.data.y, shape.data.z));
                                        break;

                                case C_CollisionShape::Shape::Cylinder:
                                        shape.shape = new btCylinderShape(btVector3(shape.data.x, shape.data.y, shape.data.z));
                                        break;

                                case C_CollisionShape::Shape::Sphere:
                                        shape.shape = new btSphereShape(shape.data.x);
                                        break;

                                case C_CollisionShape::Shape::Cone:
                                        shape.shape = new btConeShape(shape.data.x, shape.data.y);
                                        break;

                                case C_CollisionShape::Shape::Capsule:
                                        shape.shape = new btCapsuleShape(shape.data.x, shape.data.y);
                                        break;

                                default:
                                        break;
                                }
                        }
                        if (c_collision_shape.shapes.size() == 1 && c_collision_shape.shapes[0].offset == glm::vec3(0.) && c_collision_shape.shapes[0].local_rotation == glm::vec3(0.))
                        {
                                c_collision_shape.shape = c_collision_shape.shapes[0].shape;
                        }
                        else
                        {
                                auto* compound_shape = new btCompoundShape();
                                for (auto& shape : c_collision_shape.shapes)
                                        compound_shape->addChildShape(btTransform(
                                                btQuaternion(shape.local_rotation.y, shape.local_rotation.x, shape.local_rotation.z),
                                                btVector3(shape.offset.x, shape.offset.y, shape.offset.z)
                                                ), shape.shape);
                        }
                }
        }
}

void on_destroy_collision_shape(entt::registry& reg, entt::entity entity)
{
        C_CollisionShape& c_collision_shape = reg.get<C_CollisionShape>(entity);
        C_PhysicsWorld& c_physics_world = reg.ctx<C_PhysicsWorld>();
        // for (auto& shape : c_collision_shape.shapes)
        // {
        //         delete shape.shape;
        // }
        if (c_collision_shape.shape)
                delete c_collision_shape.shape;
}

void on_construct_collider(entt::registry& reg, entt::entity entity)
{
}

void on_destroy_collider(entt::registry& reg, entt::entity entity)
{
}

void on_construct_rigid_body(entt::registry& reg, entt::entity entity)
{
        C_RigidBody& c_rigid_body = reg.get<C_RigidBody>(entity);
        C_PhysicsWorld& c_physics_world = reg.ctx<C_PhysicsWorld>();

        auto* c_collision_shape = reg.try_get<C_CollisionShape>(entity);
        if (c_collision_shape)
        {
                LD_VERIFY(c_collision_shape->shape && c_collision_shape->shape->getShapeType() != INVALID_SHAPE_PROXYTYPE, "Invalid RigidBody Shape!");
                bool isDynamic = (c_rigid_body.mass != 0.f);

                btVector3 localInertia(0, 0, 0);
                if (isDynamic)
                        c_collision_shape->shape->calculateLocalInertia(c_rigid_body.mass, localInertia);
                MotionState* motion_state = new MotionState(Luddite::Entity{entity, &reg});
                btRigidBody::btRigidBodyConstructionInfo cInfo(c_rigid_body.mass, motion_state, c_collision_shape->shape, localInertia);
                c_rigid_body.rigid_body = new btRigidBody(cInfo);

                c_rigid_body.rigid_body->setUserIndex(-1);
                c_physics_world.dynamics_world->addRigidBody(c_rigid_body.rigid_body);

                // c_rigid_body.rigid_body->setAngularFactor()
        }
}

void on_destroy_rigid_body(entt::registry& reg, entt::entity entity)
{
        C_RigidBody& c_rigid_body = reg.get<C_RigidBody>(entity);
        C_PhysicsWorld& c_physics_world = reg.ctx<C_PhysicsWorld>();
        if (c_rigid_body.rigid_body)
        {
                // if (c_rigid_body.rigid_body->getMotionState())
                delete c_rigid_body.rigid_body->getMotionState();
                c_physics_world.dynamics_world->removeRigidBody(c_rigid_body.rigid_body);
        }
}

void S_Physics::Configure(Luddite::World& world)
{
        LD_PROFILE_FUNCTION();
        world.GetRegistry().on_construct<C_CollisionShape>().connect<&on_construct_collision_shape>();
        world.GetRegistry().on_destroy<C_CollisionShape>().connect<&on_destroy_collision_shape>();

        world.GetRegistry().on_construct<C_Collider>().connect<&on_construct_collider>();
        world.GetRegistry().on_destroy<C_Collider>().connect<&on_destroy_collider>();

        world.GetRegistry().on_construct<C_RigidBody>().connect<&on_construct_rigid_body>();
        world.GetRegistry().on_destroy<C_RigidBody>().connect<&on_destroy_rigid_body>();

        auto& phys_world = world.SetSingleton<C_PhysicsWorld>();
        phys_world.collision_configuration = new btDefaultCollisionConfiguration();
        phys_world.dispatcher = new btCollisionDispatcher(phys_world.collision_configuration);
        phys_world.broadphase = new btDbvtBroadphase();
        btSequentialImpulseConstraintSolver* sol = new btSequentialImpulseConstraintSolver;
        phys_world.solver = sol;
        phys_world.dynamics_world = new btDiscreteDynamicsWorld(phys_world.dispatcher, phys_world.broadphase, phys_world.solver, phys_world.collision_configuration);
        phys_world.dynamics_world->setGravity(btVector3(0, -9.8f, 0));
}

void S_Physics::Cleanup(Luddite::World& world)
{
        LD_PROFILE_FUNCTION();
        world.ClearComponent<C_RigidBody>();
        auto& phys_world = world.GetSingleton<C_PhysicsWorld>();

        //remove the rigidbodies from the dynamics world and delete them
        int i;
        for (i = phys_world.dynamics_world->getNumConstraints() - 1; i >= 0; i--)
        {
                phys_world.dynamics_world->removeConstraint(phys_world.dynamics_world->getConstraint(i));
        }
        for (i = phys_world.dynamics_world->getNumCollisionObjects() - 1; i >= 0; i--)
        {
                btCollisionObject* obj = phys_world.dynamics_world->getCollisionObjectArray()[i];
                btRigidBody* body = btRigidBody::upcast(obj);
                if (body && body->getMotionState())
                {
                        delete body->getMotionState();
                }
                phys_world.dynamics_world->removeCollisionObject(obj);
                delete obj;
        }

        delete phys_world.broadphase;
        delete phys_world.dispatcher;
        delete phys_world.solver;
        delete phys_world.collision_configuration;
}

void S_Physics::Update(Luddite::World& world, float delta_time)
{
        LD_PROFILE_FUNCTION();
        world.GetSingleton<C_PhysicsWorld>().dynamics_world->stepSimulation(delta_time);
}
