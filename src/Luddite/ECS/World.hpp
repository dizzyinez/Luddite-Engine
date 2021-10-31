#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/ECS/Entity.hpp"
#include "Luddite/ECS/System.hpp"
#include "Luddite/ECS/Components/Components.hpp"

namespace Luddite
{
template<typename ... Type>
inline constexpr entt::get_t<Type...> Borrow{};
template<typename ... Type>
inline constexpr entt::exclude_t<Type...> Exclude{};
class LUDDITE_API World
{
        public:
        World() = default;
        ~World()
        {
                for (auto& pair : m_Systems)
                        pair.second->Cleanup(*this);
        }

        inline Entity CreateEntity() {return {m_Registry.create(), &m_Registry};}
        inline Entity CreateEntity(Luddite::EntityID hint) {return {m_Registry.create(hint), &m_Registry};}
        inline Entity NullEntity() {return {Luddite::NullEntityID, &m_Registry};}

        inline void DestroyEntityFromID(Luddite::EntityID id) {m_Registry.destroy(id);}

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
        void UpdateSystemFixed(Args && ... args)
        {
                GetSystem<T>().FixedUpdate(std::forward<Args>(args)...);
        }
	
        template <typename T, typename ... Args>
        void UpdateSystem(Args && ... args)
        {
                GetSystem<T>().Update(std::forward<Args>(args)...);
        }

        void ConfigureSystems()
        {
                for (auto& pair : m_Systems)
                        pair.second->Configure(*this);
        }

        template <typename ... Component, typename ... Exclude>
        auto GetView(entt::exclude_t<Exclude...> E = {})
        {
                return m_Registry.view<Component...>(entt::exclude<Exclude...>);
        }

        template<typename ... Owned, typename ... Borrowed, typename ... Exclude>
        auto GetGroup(entt::get_t<Borrowed...> B = {}, entt::exclude_t<Exclude...> E = {})
        {
                return m_Registry.group<Owned...>(entt::get<Borrowed...>, entt::exclude<Exclude...>);
        }

        template <typename Func>
        void each(Func func) const
        {
                return m_Registry.each(std::forward<Func>(func));
        }

        template <typename ... Components>
        void ClearComponent()
        {
                m_Registry.clear<Components...>();
        }

        Entity GetEntityFromID(EntityID id)
        {
                return Entity(id, &m_Registry);
        }

        bool IsEntityIDValid(EntityID id) const
        {
                return m_Registry.valid(id);
        }


        template <typename ... Components>
        void CloneTo(World& to) const
        {
                //ensure the destination world is empty
                to.ClearComponent<Components...>();
                const auto* to_data = to.GetRegistry().data();
                const auto to_size = to.GetRegistry().size();
                to.GetRegistry().destroy(to_data, to_data + to_size);

                CloneToEmpty<Components...>(to);
        }

        template <typename ... Components>
        void CloneToEmpty(World& to) const
        {
                //assign the entities that the from world has to the destination world
                const auto* from_data = m_Registry.data();
                const auto from_size = m_Registry.size();
                to.GetRegistry().assign(from_data, from_data + from_size, entt::null);

                CloneComponent<Components...>(to);
        }

        template <typename Component>
        void CloneComponent(World& to) const
        {
                const auto view = m_Registry.view<const Component>();
                const auto* data = view.data();
                const auto size = view.size();
                if (size == 0)
                        return;



                LD_LOG_INFO("Cloning: {}, value: {}, size: {}", typeid(Component).name(), entt::type_seq<Component>::value(), size);
                if constexpr (std::is_empty<Component>::value)
                {
                        LD_LOG_INFO("Component is empty: {}", typeid(Component).name());
                        to.m_Registry.insert<Component>(data, data + size);
                }
                else
                {
                        // const auto* raw = view.raw();
                        auto* raw = view.raw();
                        to.m_Registry.insert<Component>(data, data + size, *raw);
                }
        }

        template <typename Component, typename Component2, typename ... Rest>
        void CloneComponent(World& to) const
        {
                CloneComponent<Component>(to);
                CloneComponent<Component2, Rest...>(to);
        }

        template <typename ... Singleton>
        void CloneSingletonsTo(World& to) const
        {
                to.ClearComponent<Singleton...>();
                CloneSingletonsToEmpty<Singleton...>(to);
        }

        template <typename ... Singleton>
        void CloneSingletonsToEmpty(World& to) const
        {
                CloneSingleton<Singleton...>(to);
        }

        template <typename Singleton>
        void CloneSingleton(World& to) const
        {
                const Singleton* singleton = m_Registry.try_ctx<const Singleton>();
                if (singleton)
                        to.SetSingleton<Singleton>(*singleton);
        }

        template <typename Singleton, typename Singleton2, typename ... Rest>
        void CloneSingleton(World& to) const
        {
                CloneSingleton<Singleton>(to);
                CloneSingleton<Singleton2, Rest...>(to);
        }

        // template <typename Component>
        // auto OnConstruct() {return m_Registry.on_construct<Component>();}

        // template <typename Component>
        // auto OnDestroy() {return m_Registry.on_destroy<Component>();}

        // template <typename Component>
        // auto OnUpdate() {return m_Registry.on_update<Component>();}
        // template <>
        // void CloneComponent<C_Model>(World & to) const
        // {
        // }

        entt::registry& GetRegistry() {return m_Registry;}

        private:
        entt::registry m_Registry{};
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
