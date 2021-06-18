#include "Luddite/ECS/World.hpp"
#include "Luddite/ECS/Components/Components.hpp"
namespace Luddite
{
World::World()
{}
World::~World()
{}

// template <typename Component>
// void World::CloneComponent(World& to) const
// {
//         const auto view = m_Registry.view<const Component>();
//         const auto* data = view.data();
//         const auto size = view.size();

//         if constexpr (std::is_empty<Component>::value)
//         {
//                 to.m_Registry.insert<Component>(data, data + size);
//         }
//         else
//         {
//                 const auto* raw = view.raw();
//                 to.m_Registry.insert<Component>(data, data + size, raw, raw + size);
//         }
// }

// template <>
// void World::CloneComponent<C_Transform3D>(World& to) const;

// template <>
// void World::CloneComponent<C_Transform3D>(World& to) const
// {
//         LD_LOG_INFO("TESTC_TRANSFORM3D");
// }
// template <>
// void World::CloneComponent<C_ActiveCamera>(World & to) const
// {
//         to.SetSingleton<C_ActiveCamera>(m_Registry.ctx<const C_ActiveCamera>());
//         LD_LOG_INFO("TESTC_MODEL");
// }
// template <>
// void World::CloneComponent<C_Model>(World & to) const
// {
//         LD_LOG_INFO("TESTC_MODEL");
// }
}