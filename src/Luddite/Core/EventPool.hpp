#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/Core/Event.hpp"

namespace Luddite
{
template <typename T>
class LUDDITE_API EventList
{
public:
        void Clear() {m_EventVector.clear();}

        void DispatchEvent(T& event) {m_EventVector.push_back(event);}
        void DispatchEvent(const T& event) {m_EventVector.push_back(event);}
        template<typename ... Args>
        void DispatchEvent(Args ... args) {m_EventVector.emplace_back(std::forward<Args>(args)...);}

        class LUDDITE_API unhandled_iterator : public std::vector<T>::iterator
        {
public:
                unhandled_iterator() = default;
                unhandled_iterator(typename std::vector<T>::iterator base, typename std::vector<T>::iterator end)
                        : std::vector<T>::iterator(base), _End(end)
                {
                        while (*this != _End && ((**this).handled))
                                ++*this;
                }
                unhandled_iterator& operator++()
                {
                        while (*this != _End && ((**this).handled))
                                std::vector<T>::iterator::operator++();
                        return *this;
                }
private:
                typename std::vector<T>::iterator _End;
        };

        unhandled_iterator begin() {return unhandled_iterator(m_EventVector.begin(), m_EventVector.end());}
        auto end() {return m_EventVector.end();}
private:
        std::vector<T> m_EventVector;
};

class LUDDITE_API Window;
class LUDDITE_API EventPool
{
public:
        ~EventPool()
        {
                for (const auto& [key, value] : m_EventMap)
                {
                        delete value;
                }
        }


        template <typename T>
        EventList<T>& GetList()
        {
                // std::lock_guard<std::mutex> lock(m_Mutex);
                auto it = m_EventMap.find(T::EventID());
                if (it == m_EventMap.end())
                {
                        auto pair = m_EventMap.emplace(T::EventID(), reinterpret_cast<EventList<BaseEvent>* >(new EventList<T>()));
                        LD_VERIFY(pair.second, "Unable to create Event list");
                        it = pair.first;
                }
                return *reinterpret_cast<EventList<T>* >(it->second);
        }

private:
        friend class Window;
        inline void Clear() {
                for (auto pair : m_EventMap)
                        pair.second->Clear();
        }
        std::unordered_map<EventIDType, EventList<BaseEvent>* > m_EventMap;
        // mutable std::mutex m_Mutex;
};
}