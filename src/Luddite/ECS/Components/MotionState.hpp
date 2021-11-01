#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/Core/RingBuffer.hpp"
#include "Luddite/ECS/Components/Transform.hpp"

//template <std::size_t max_age>
struct C_MotionState
{
	Luddite::RingBuffer<C_Transform3D, 64> TransformsRingBuffer;
};
