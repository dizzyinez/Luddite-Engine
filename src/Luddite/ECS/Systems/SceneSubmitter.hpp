#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/ECS/Components/Transform.hpp"
#include "Luddite/ECS/System.hpp"
#include "Luddite/Core/Profiler.hpp"
#include "Luddite/ECS/World.hpp"
#include "Luddite/Graphics/Lights.hpp"
#include "Luddite/Graphics/RenderTarget.hpp"
#include "Luddite/Graphics/Renderer.hpp"

class S_SceneSubmitter : public Luddite::System<S_SceneSubmitter>
{
        public:

        void Update(Luddite::World& world, float lerp_alpha)
        {
                LD_PROFILE_FUNCTION();
                Luddite::Renderer::BeginScene();
                for (const auto &&[id, transform, model]: world.GetGroup<C_Transform3D>(Luddite::Borrow<C_Model>).each())
                {
                        std::vector<glm::mat4> node_transforms;
                        node_transforms.reserve(model.ModelHandle->m_Nodes.size());
                        for (auto& node : model.ModelHandle->m_Nodes)
                        {
                                if (node.m_ParentNodeID >= 0)
                                        node_transforms.push_back(node.m_Transform);
                                else
                                        node_transforms.push_back(node.m_Transform * node_transforms[node.m_ParentNodeID]);
                        }
                        for (auto& pair : model.ModelHandle->m_MeshNodePairs)
                        {
                                auto& mesh = model.ModelHandle->m_Meshes[pair.m_MeshIndex];
                                Luddite::Renderer::SubmitMesh(&mesh, transform.GetTransformMatrix() * node_transforms[pair.m_NodeIndex], model.ModelHandle->m_Materials[mesh.m_MaterialIndex]);
                        }
                        // for (auto& mesh : model.ModelHandle->m_Meshes)
                        // {
                        //         Luddite::Renderer::SubmitMesh(&mesh, transform.GetTransformMatrix());
                        // }
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
		for (const auto &&[id, spot_light, transform] : world.GetGroup<C_SpotLight>(Luddite::Borrow<C_Transform3D>).each())
		{
			Luddite::SpotLightCPU light;
			light.Position = glm::vec4(transform.Translation, 1.f);
			light.Color = spot_light.Color.GetVec3();
			light.Range = spot_light.Range;
			light.Intensity = spot_light.Intensity;
			light.HalfAngle = spot_light.HalfAngle;
			light.Direction = glm::vec4(transform.GetLookDirection(), 0.0f);
			Luddite::Renderer::SubmitSpotLight(light);
		}
		for (const auto &&[id, directional_light, transform] : world.GetGroup<C_DirectionalLight>(Luddite::Borrow<C_Transform3D>).each())
		{
			Luddite::DirectionalLightCPU light;
			light.Direction = glm::vec4(transform.GetLookDirection(), 0.0f);
			light.Color = directional_light.Color.GetVec3();
			light.Intensity = directional_light.Intensity;
			Luddite::Renderer::SubmitDirectionalLight(light);
		}
                Luddite::Renderer::EndScene();
        }
};
