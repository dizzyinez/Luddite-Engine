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

                //w.component<RenderPipeline>();
                auto pre_render = w.component<PreRender       >();
                auto on_render = w.component<OnRender         >();
                auto post_render = w.component<PostRender     >();

                //w.component<SimulationPipeline                >();
                auto pre_simulate = w.component<PreSimulate   >();
                auto on_simulate = w.component<OnSimulate     >();
                auto post_simulate = w.component<PostSimulate >();

                w.component<InputPipeline>("InputPipeline")
                .emplace<flecs::pipeline>(
                        w.pipeline("Input Pipeline")
                        .add(on_input)
                        .add(post_input));

                w.component<UpdatePipeline>("UpdatePipeline")
                .emplace<flecs::pipeline>(
                        w.pipeline("Update Pipeline")
                        .add(pre_update)
                        .add(on_update)
                        .add(post_update));

                w.component<RenderPipeline>("RenderPipeline")
                .emplace<flecs::pipeline>(
                        w.pipeline("Render Pipeline")
                        .add(pre_render)
                        .add(on_render)
                        .add(post_render));

                w.component<SimulationPipeline>("SimulationPipeline")
                .emplace<flecs::pipeline>(
                        w.pipeline("Simulation Pipeline")
                        .add(pre_simulate)
                        .add(on_simulate)
                        .add(post_simulate));

                w.id<Scene>().entity();
                w.id<Prefabs>().entity();

                w.component<ReflectionData>();
        }
};
}
