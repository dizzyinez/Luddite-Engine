#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/ECS/System.hpp"

class S_SceneSubmitter : public Luddite::System<S_SceneSubmitter>
{
        public:
        void configure()
        {
                // world.GetGroup<C_Transform3D>(Luddite::Borrow<C_Model>);
        }

        void Update(Luddite::World& world, float alpha)
        {
                Luddite::Renderer::BeginScene();
                for (const auto &&[id, transform, model]: world.GetGroup<C_Transform3D>(Luddite::Borrow<C_Model>).each())
                {
                        for (auto& mesh : model.ModelHandle->meshes)
                                Luddite::Renderer::SubmitMesh(mesh, transform.GetTransformMatrix());
                }
                Luddite::Renderer::EndScene();
        }
};