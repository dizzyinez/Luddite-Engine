#pragma once
#include "Luddite/Core/DebugTools.hpp"
#include "Luddite/Core/pch.hpp"
#include <array>

namespace Luddite
{
template <typename T, std::size_t size>
class LUDDITE_API RingBuffer
{
	public:
	void push(const T& t) 
	{
		head++;
		if (head == size)
			head = 0;
		m_Array[head] = t;
	}

	T* get(std::size_t age)
	{
		LD_VERIFY(age < size, "Tried to access element of age {}, but the ring buffer size was {}", age, size);
		std::size_t index = head - age;
		if (index < 0)
			index += size;
		return &m_Array[index];
	}

	void fill(const T& t)
	{
		std::fill(m_Array.data(), m_Array.data() + m_Array.size(), t);
	}
	static std::size_t getMaxSize() {return size;}
	private:
	std::size_t head = 0;
	std::array<T, size> m_Array;
};
}
