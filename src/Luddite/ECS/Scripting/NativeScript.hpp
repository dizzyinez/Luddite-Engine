#pragma once
#include "Luddite/Core/pch.hpp"
//#include "Luddite/ECS/World.hpp"
#include "Luddite/ECS/Entity.hpp"
#include "Luddite/Core/RingBuffer.hpp"

namespace Luddite
{
class World;
class LUDDITE_API INativeScript
{
	public:
	virtual void Increment() {}
	virtual void OnInput() {}
	virtual void OnUpdate(float delta_time, World& world, Entity entity) {}
	virtual void OnFixedUpdate(float delta_time, World& world, Entity entity) {}
	private:
};
#define DEFAULT_MAX_SCRIPT_RING_BUFFER_SIZE 32
template <typename D, std::size_t size = DEFAULT_MAX_SCRIPT_RING_BUFFER_SIZE>
class LUDDITE_API StateScript : INativeScript
{
	public:
	virtual void Increment() override
	{
		m_Data.push(D{});
	}
	private:
	D* getData(std::size_t age = 0) { return m_Data.getData(age); }
	RingBuffer<D, size> m_Data;
};
}
