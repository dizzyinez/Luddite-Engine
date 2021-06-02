#pragma once
#include "Luddite/Core/pch.hpp"

namespace Luddite
{
template <typename T>
struct LUDDITE_API Queue
{
public:
        Queue() = default;
        Queue(const Queue<T>&) = delete;
        Queue& operator=(const Queue<T>&) = delete;
        Queue(Queue<T>&& old)
        {
                std::lock_guard<std::mutex> lock(m_Mutex);
                m_Queue = std::move(old.m_Queue);
        }

        std::size_t size() const
        {
                std::lock_guard<std::mutex> lock(m_Mutex);
                return m_Queue.size();
        }

        bool empty() const
        {
                std::lock_guard<std::mutex> lock(m_Mutex);
                return m_Queue.empty();
        }

        std::optional<T> pop()
        {
                std::lock_guard<std::mutex> lock(m_Mutex);
                if (m_Queue.p_empty())
                        return {};
                T result = m_Queue.front();
                m_Queue.pop();
                return result;
        }

        void push(const T& item)
        {
                std::lock_guard<std::mutex> lock(m_Mutex);
                m_Queue.push(item);
        }

        template <typename ... Args>
        void emplace(Args&& ... args)
        {
                std::lock_guard<std::mutex> lock(m_Mutex);
                m_Queue.emplace(std::forward<Args>(args)...);
        }

        void clear()
        {
                std::lock_guard<std::mutex> lock(m_Mutex);
                m_Queue.clear();
        }

private:
        inline bool p_empty() const {return m_Queue.empty();}
        std::queue<T> m_Queue;
        mutable std::mutex m_Mutex;
};
}