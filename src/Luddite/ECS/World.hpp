#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/ECS/Entity.hpp"
#include "Luddite/ECS/System.hpp"

namespace Luddite
{
class LUDDITE_API World
{
        public:
        World();
        ~World();

        inline Entity CreateEntity() {return {m_Registry.create(), &m_Registry};}

        template <typename T, typename ... Args>
        inline T& SetSingleton(Args && ... args)
        {
                return m_Registry.set<T>(std::forward<Args>(args)...);
        }

        template <typename T>
        inline T* TrySingleton()
        {
                return m_Registry.try_ctx<T>();
        }

        template <typename T>
        inline T& GetSingleton()
        {
        #ifdef LD_DEBUG
                T* pSingleton = TrySingleton<T>();
                LD_VERIFY(pSingleton, "World doesn't have singleton component {}", typeid(T).name());
                return *pSingleton;
        #else
                return m_Registry.ctx<T>();
        #endif //LD_DEBUG
        }

        template <typename T, typename ... Args>
        void RegisterSystem(Args && ... args)
        {
                m_Systems.emplace(T::SystemID(), std::make_unique<T>(std::forward<Args>(args)...));
        }

        template <typename T, typename ... Args>
        void UpdateSystem(Args && ... args)
        {
                //TODO: instrumentation here
                GetSystem<T>().Update(std::forward<Args>(args)...);
        }

        void ConfigureSystems()
        {
                for (auto& pair : m_Systems)
                {
                        pair.second->Configure();
                }
        }

        template<typename ... Owned, typename ... Borrowed, typename ... Exclude>
        auto GetGroup(entt::get_t<Borrowed...> B = {}, entt::exclude_t<Exclude...> E = {})
        {
                return m_Registry.group<Owned...>(entt::get<Borrowed...>, entt::exclude<Exclude...>);
        }

        Entity GetEntityFromID(EntityID id)
        {
                return Entity(id, &m_Registry);
        }
        private:
        entt::registry m_Registry;
        std::unordered_map<SystemIDType, std::unique_ptr<BaseSystem> > m_Systems;

        template <typename T>
        T& GetSystem()
        {
                auto it = m_Systems.find(T::SystemID());
                LD_VERIFY(it != m_Systems.end(), "World doesn't have system {}", typeid(T).name());
                return *static_cast<T*>(it->second.get());
        }
};
}