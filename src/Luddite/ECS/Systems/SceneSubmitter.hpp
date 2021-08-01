#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/ECS/System.hpp"

class S_SceneSubmitter : public Luddite::System<S_SceneSubmitter>
{
        public:

        void Update(Luddite::World& world, float lerp_alpha)
        {
                Luddite::Renderer::BeginScene();
                for (const auto &&[id, transform, model]: world.GetGroup<C_Transform3D>(Luddite::Borrow<C_Model>).each())
                {
                        for (auto& mesh : model.ModelHandle->meshes)
                        {
                                Luddite::Renderer::SubmitMesh(&mesh, transform.GetTransformMatrix());
                        }
                }
                for (const auto &&[id, point_light, transform] : world.GetGroup<C_PointLight>(Luddite::Borrow<C_Transform3D>).each())
                {
                        Luddite::PointLightCPU light;
                        light.Position = glm::vec4(transform.Translation, 1.f);
                        light.Color = point_light.Color.GetVec3();
                        light.Range = point_light.Range;
                        light.Intensity = point_light.Intensity;
                        Luddite::Renderer::SubmitPointLight(light);
                }
                Luddite::Renderer::EndScene();
        }
};