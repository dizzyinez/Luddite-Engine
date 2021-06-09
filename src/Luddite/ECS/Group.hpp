#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/ECS/Entity.hpp"

namespace Luddite
{
// template <typename ... T>
// using Borrowed = entt::get_t<T...>;
template<typename ... Type>
inline constexpr entt::get_t<Type...> Borrow{};
template<typename ... Type>
inline constexpr entt::exclude_t<Type...> Exclude{};


// // template<typename Entity, typename ... Exclude, typename ... Get, typename ... Owned>
// // class basic_group<Entity, exclude_t<Exclude...>, get_t<Get...>, Owned...> final
// // {
// // }
// template<typename ... Exclude, typename ... Borrowed, typename ... Owned>
// class Group<entt::exclude_t<Exclude...>, entt::get_t<Borrowed...>, Owned...>
// {
//         private:
//         using entt_group_type = entt::basic_group<entt::entity, entt::exclude_t<Exclude...>, entt::get_t<Borrowed...>, Owned... >;
//         // using entt_iterator_type = typename entt_group_type::iterator;
//         class input_iteration_group
//         {
//                 using entt_iterator_type = typename entt_group_type::iterator;
//                 public:
//                 class iterator
//                 {
//                         public:
//                         using difference_type = std::ptrdiff_t;
//                         using value_type = decltype(std::tuple_cat(std::tuple<Entity>{}, std::declval<entt_group_type>().get({})));
//                         using pointer = void;
//                         using reference = value_type;
//                         using iterator_category = std::input_iterator_tag;
//                         iterator(entt_iterator_type&& iterator)
//                                 : m_EnttIterator{iterator}
//                         {}

//                         iterator& operator++()
//                         {
//                                 m_EnttIterator++;
//                                 return *this;
//                         }
//                         reference operator*() const
//                         {
//                                 return std::tuple_cat(
//                                         std::make_tuple(Entity)
//                                         );
//                         }
//                         private:
//                         entt_iterator_type m_EnttIterator;
//                 };

//                 input_iteration_group(typename entt_group_type::iterable_group& iter_group)
//                         : m_EnttIterableGroup{iter_group}
//                 {}
//                 iterator begin()
//                 {
//                         return iterator{m_EnttIterableGroup.begin()};
//                 }

//                 iterator end()
//                 {
//                         return iterator{m_EnttIterableGroup.end()};
//                 }
//                 private:
//                 iterator begin;
//                 iterator end;
//                 typename entt_group_type::iterable_group m_EnttIterableGroup;
//         };
//         public:
//         Group(entt_group_type&& group)
//                 : m_EnttGroup{group}
//         {}

//         input_iteration_group each()
//         {
//                 auto&& each = m_EnttGroup.each();
//                 input_iteration_group
//                 // each.begin();
//                 // return input_iteration_group(());
//         }

//         // iterator begin()
//         // {
//         //         return iterator{m_EnttGroup.each().begin()};
//         // }
//         // iterator end()
//         // {
//         //         return iterator{m_EnttGroup.each().end()};
//         // }
//         private:
//         entt_group_type m_EnttGroup;
// };
}