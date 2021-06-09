#pragma once
#include "Luddite/Core/pch.hpp"

namespace Luddite
{
using EntityID = entt::entity;
constexpr EntityID NullEntityID = entt::null;
class LUDDITE_API Entity
{
        public:
        Entity() = default;
        Entity(EntityID EntityID_, entt::registry* pRegistry_)
                : m_EntityID{EntityID_}, m_pRegistry{pRegistry_} {}

        /**
         * @brief Add the given component to an entity.
         *
         * @tparam T
         * @tparam Args
         * @param args Component constructor arguments
         * @return T&
         */
        template <typename T, typename ... Args>
        inline T& AddComponent(Args && ... args)
        {
                return m_pRegistry->emplace<T>(m_EntityID, std::forward<Args>(args)...);
        }

        /**
         * @brief Remove a component from the entity
         *
         * @tparam T
         */
        template <typename T>
        inline void RemoveComponent()
        {
                m_pRegistry->remove<T>();
        }

        /**
         * @brief Destroys the entity and all of its components.
         *
         */
        inline void Destroy()
        {
                m_pRegistry->destroy(m_EntityID);
                m_EntityID = entt::null;
        }

        // inline void Exists()

        /**
         * @brief Returns a pointer to the given component, or nullptr if the entity doesn't have the component.
         *
         * @tparam T
         * @return T*
         */
        template <typename T>
        inline T* TryComponent() const
        {
                return m_pRegistry->try_get<T>(m_EntityID);
        }

        /**
         * @brief Gets a reference to the given component. Use ReplaceComponent() if you want to change the component's data.
         *
         * @tparam T
         * @return T&
         */
        template <typename T>
        inline const T& GetComponent() const
        {
        #ifdef LD_DEBUG
                T* pComponent = TryComponent<T>();
                LD_VERIFY(pComponent, "Entity (id: {}) doesn't have component {}", m_EntityID, GET_TYPENAME_STRING(T));
                return *pComponent;
        #else
                return m_pRegistry->get<T>(m_EntityID);
        #endif //LD_DEBUG
        }

        /**
         * @brief Replaces the given component. This is the prefered way to update components
         * as it triggers calls to let the engine know the component has updated.
         * @tparam T
         * @tparam Args
         * @param args
         */
        template <typename T, typename ... Args>
        inline void ReplaceComponent(Args && ... args) const
        {
                m_pRegistry->replace<T>(m_EntityID, std::forward<Args>(args)...);
        }

        /**
         * @brief Gets the entity's ID
         *
         * @return EntityID
         */
        inline EntityID GetID() const {return m_EntityID;}
        private:
        EntityID m_EntityID = entt::null;
        entt::registry* m_pRegistry = nullptr;
};
}