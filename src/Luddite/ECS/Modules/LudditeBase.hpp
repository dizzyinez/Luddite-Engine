#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/ECS/Reflection.hpp"

namespace Luddite
{
//Pipelines
struct InputPipeline {};
struct OnInput {};
struct PostInput {};

struct UpdatePipeline {};
struct PreUpdate {};
struct OnUpdate {};
struct PostUpdate {};

struct RenderPipeline {};
struct PreRender {};
struct OnRender {};
struct PostRender {};

struct SimulationPipeline {};
struct PreSimulate {};
struct OnSimulate {};
struct PostSimulate {};

struct Pipeline
{
        //flecs::pipeline
};

//Heirarchies
struct Scene {};
struct Prefabs {};

//Meta
struct ReflectionData
{
        void (*ImGuiDraw)(void* data);
};

struct Components
{
        Components(flecs::world& w)
        {
                w.module<Components>();
                //w.component<InputPipeline>();
                auto on_input = w.component<OnInput           >();
                auto post_input = w.component<PostInput       >();

                //w.component<UpdatePipeline>();
                auto pre_update = w.component<PreUpdate       >();
                auto on_update = w.component<OnUpdate         >();
                auto post_update = w.component<PostUpdate     >();

                //w.component<SimulationPipeline                >();
                auto pre_simulate = w.component<PreSimulate   >();
                auto on_simulate = w.component<OnSimulate     >();
                auto post_simulate = w.component<PostSimulate >();

                //w.component<SimulationPipeline                >();
                auto pre_render = w.component<PreRender   >();
                auto on_render = w.component<OnRender     >();
                auto post_render = w.component<PostRender >();

                flecs::pipeline(w, w.id<InputPipeline>().entity())
                .add(on_input)
                .add(post_input);

                flecs::pipeline(w, w.id<UpdatePipeline>().entity())
                .add(pre_update)
                .add(on_update)
                .add(post_update);

                flecs::pipeline(w, w.id<SimulationPipeline>().entity())
                .add(pre_simulate)
                .add(on_simulate)
                .add(post_simulate);

                flecs::pipeline(w, w.id<RenderPipeline>().entity())
                .add(pre_render)
                .add(on_render)
                .add(post_render);

                w.id<Scene>().entity();
                w.id<Prefabs>().entity();

                w.component<ReflectionData>();
        }
};

struct Systems
{
        Systems(flecs::world& w)
        {
                w.import<Luddite::Components>();
                w.module<Systems>();
                w.system<>("Input Pipeline Dummy System")
                .kind(w.id<OnInput>()).iter([](flecs::iter it){});
                w.system<>("Update Pipeline Dummy System")
                .kind(w.id<OnUpdate>()).iter([](flecs::iter it){});
                w.system<>("Simulate Pipeline Dummy System")
                .kind(w.id<OnSimulate>()).iter([](flecs::iter it){});
                w.system<>("Render Pipeline Dummy System")
                .kind(w.id<OnRender>()).iter([](flecs::iter it){});
        }
};
}
