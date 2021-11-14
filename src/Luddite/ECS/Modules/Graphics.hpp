#pragma once
#include "Luddite/Core/DebugTools.hpp"
#include "Luddite/Core/pch.hpp"
#include "Luddite/Graphics/Camera.hpp"
#include "Luddite/Graphics/Color.hpp"
#include "Luddite/Core/AssetTypes/Model.hpp"
#include "Luddite/ECS/Modules/LudditeBase.hpp"
#include "Luddite/ECS/Modules/Transform3D.hpp"
#include "Luddite/Graphics/Lights.hpp"
#include "Luddite/Graphics/RenderTarget.hpp"
#include "Luddite/Graphics/Renderer.hpp"
#include "flecs/addons/cpp/flecs.hpp"

namespace Graphics
{
struct MainWindow
{
        Luddite::RenderTarget RenderTarget;
};
struct RenderToMainWindow {};
struct RenderTarget
{
        Luddite::RenderTarget RenderTarget;
};

struct Camera
{
        Luddite::Camera::ProjectionType Projection = Luddite::Camera::ProjectionType::PERSPECTIVE;
        float FOV = glm::radians(90.f);
        float OrthoScale = 100.f;
        float ClipNear = 0.1f;
        float ClipFar = 1000.f;
};

struct PointLight
{
        Luddite::ColorRGB Color;
        float Range = 10.f;
        float Intensity = 1.f;
};

struct SpotLight
{
        Luddite::ColorRGB Color;
        float Range = 10.f;
        glm::vec3 Direction;
        float Intensity = 1.f;
        float HalfAngle = glm::quarter_pi<float>();
};

struct DirectionalLight
{
        Luddite::ColorRGB Color;
        float Intensity = 1.f;
};

struct Model
{
        Luddite::Handle<Luddite::Model> ModelHandle;
};

struct Components
{
        Components(flecs::world& w)
        {
                w.module<Components>();
                w.component<MainWindow>("Main Window");
                w.set<MainWindow>({});
                w.component<RenderToMainWindow>("Render To Main Window");
                w.component<Camera>("Camera");
                w.component<RenderTarget>("Render Target");
                w.component<PointLight>("Point Light");
                w.component<SpotLight>("Spot Light");
                w.component<DirectionalLight>("Directional Light");
                w.component<Model>("Model");
                //w.id<MainWindow>().entity().add<RenderTarget>();
        }
};

struct Systems
{
        Systems(flecs::world& w)
        {
                w.import<Luddite::Components>();
                w.import<Transform3D::Components>();
                w.import<Components>();
                w.module<Systems>();

                w.system<const Camera, const RenderToMainWindow>("Assign Main Window")
                .kind(w.id<Luddite::PreRender>())
                .term<RenderTarget>().inout(flecs::Out)
                .term<const MainWindow>().inout(flecs::In).singleton()
                .iter([](flecs::iter it){
                                auto rt = it.term<RenderTarget>(3);
                                auto mw = it.term<const MainWindow>(4);
                                for (auto i : it)
                                {
                                        rt[i].RenderTarget = mw->RenderTarget;
                                }
                        });

                w.system<>("Begin Scene")
                .kind(w.id<Luddite::OnRender>())
                .iter([](flecs::iter it){
                                Luddite::Renderer::BeginScene();
                        });

                w.system<const Transform3D::Translation, const Transform3D::Rotation, const Transform3D::Scale, const Model>()
                .kind(w.id<Luddite::OnRender>())
                .each([](flecs::entity e, const Transform3D::Translation& t, const Transform3D::Rotation& r, const Transform3D::Scale& s, const Model& m){
                                glm::mat4 node_transforms[m.ModelHandle->m_Nodes.size()];
                                int idx = 0;
                                for (auto& node : m.ModelHandle->m_Nodes)
                                {
                                        LD_VERIFY(node.m_ParentNodeID < idx, "Order of meshes in model {} are not in the right order!");
                                        if (node.m_ParentNodeID == -1)
                                                //if (true)
                                                node_transforms[idx] = node.m_Transform;
                                        else
                                                node_transforms[idx] = node.m_Transform * node_transforms[node.m_ParentNodeID];
                                        //node_transforms[idx] = node_transforms[node.m_ParentNodeID] * node.m_Transform;
                                        idx++;
                                }
                                for (auto& pair : m.ModelHandle->m_MeshNodePairs)
                                {
                                        auto& mesh = m.ModelHandle->m_Meshes[pair.m_MeshIndex];
                                        Luddite::Renderer::SubmitMesh(&mesh, t.GetMatrix() * r.GetMatrix() * s.GetMatrix() * node_transforms[pair.m_NodeIndex], m.ModelHandle->m_Materials[mesh.m_MaterialIndex]);
                                }
                        });

                w.system<const Transform3D::Translation, const PointLight>()
                .kind(w.id<Luddite::OnRender>())
                .each([](flecs::entity e, const Transform3D::Translation& t, const PointLight& l){
                                Luddite::PointLightCPU light;
                                light.Position = glm::vec4(t.Translation, 1.f);
                                light.Color = l.Color.GetVec3();
                                light.Range = l.Range;
                                light.Intensity = l.Intensity;
                                Luddite::Renderer::SubmitPointLight(light);
                        });

                w.system<const Transform3D::Translation, const Transform3D::Rotation, const SpotLight>()
                .kind(w.id<Luddite::OnRender>())
                .each([](flecs::entity e, const Transform3D::Translation& t, const Transform3D::Rotation& r, const SpotLight& l){
                                Luddite::SpotLightCPU light;
                                light.Position = glm::vec4(t.Translation, 1.f);
                                light.Color = l.Color.GetVec3();
                                light.Range = l.Range;
                                light.Intensity = l.Intensity;
                                light.HalfAngle = l.HalfAngle;
                                light.Direction = glm::vec4(r.GetLookDirection(), 0.0f);
                                Luddite::Renderer::SubmitSpotLight(light);
                        });

                w.system<const Transform3D::Rotation, const DirectionalLight>()
                .kind(w.id<Luddite::OnRender>())
                .each([](flecs::entity e, const Transform3D::Rotation& r, const DirectionalLight& l){
                                Luddite::DirectionalLightCPU light;
                                light.Color = l.Color.GetVec3();
                                light.Intensity = l.Intensity;
                                light.Direction = glm::vec4(r.GetLookDirection(), 0.0f);
                                Luddite::Renderer::SubmitDirectionalLight(light);
                        });

                w.system<>("End Scene")
                .kind(w.id<Luddite::OnRender>())
                .iter([](flecs::iter it){
                                Luddite::Renderer::EndScene();
                        });

                w.system<const Camera, const Transform3D::Translation, const Transform3D::Rotation>("Render Active Cameras")
                .term<const RenderTarget>().inout(flecs::In)
                .kind(w.id<Luddite::OnRender>())
                .iter([](flecs::iter it){
                                //.each([](flecs::entity e, const Camera& c, const Transform3D::Translation& t, const Transform3D::Rotation& r, const RenderTarget& rt){
                                auto c = it.term<const Camera>(1);
                                auto t = it.term<const Transform3D::Translation>(2);
                                auto r = it.term<const Transform3D::Rotation>(3);
                                auto rt = it.term<const RenderTarget>(4);
                                for (auto i: it)
                                {
                                        Luddite::Camera camera
                                        {
                                                t[i].Translation,
                                                r[i].GetLookDirection(),
                                                c[i].Projection,
                                                c[i].FOV,
                                                c[i].OrthoScale,
                                                c[i].ClipNear,
                                                c[i].ClipFar
                                        };
                                        Luddite::Renderer::Draw(rt[i].RenderTarget, camera);
                                }
                        });
        }
};
}
