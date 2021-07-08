#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/ECS/System.hpp"

class S_Physics : public Luddite::System<S_Physics>
{
        public:
        void Configure(Luddite::World& world) override;
        void Cleanup(Luddite::World& world) override;
        void Update(Luddite::World& world, float delta_time);
};