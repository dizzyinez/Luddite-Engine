#include "Luddite/Core/pch.hpp"
#include "btBulletDynamicsCommon.h"

struct C_CollisionShape
{
        enum class Shape : uint8_t
        {
                Box = 0,
                Sphere,
                Capsule,
                Cylinder,
                Cone
        };
        struct CollisionShape
        {
                Shape type = Shape::Box;
                glm::vec3 data = glm::vec3(0.);
                glm::vec3 offset = glm::vec3(0.);
                glm::vec3 local_rotation = glm::vec3(0.);
                // x, radius
                // y, height
                // z
                btCollisionShape* shape = nullptr;

                // ~CollisionShape()
                // {
                //         if (shape)
                //                 delete shape;
                // }
        };
        std::vector<CollisionShape> shapes;
        btCollisionShape* shape = nullptr;
};