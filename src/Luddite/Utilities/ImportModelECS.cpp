#include "Luddite/Utilities/ImportModelECS.hpp"
#include "Luddite/ECS/Modules/LudditeBase.hpp"
#include "Luddite/ECS/Modules/Transform3D.hpp"
#include "Luddite/ECS/Modules/Graphics.hpp"
#include "Luddite/ECS/Modules/Models.hpp"

namespace Luddite::Utils
{
void ImportMesh(Mesh& mesh, flecs::world& w)
{
}
flecs::entity ImportModelECS(Handle<Model> model, flecs::world& w, flecs::entity parent, bool separate_meshes)
{
        auto& root_node = model->m_Nodes[0];
        auto root = w.entity(root_node.m_Name.c_str())
                    .child_of(parent)
                    .set<Transform3D::Translation>({{root_node.m_Pos}})
                    .set<Transform3D::Rotation>({{root_node.m_Rot}})
                    .set<Transform3D::Scale>({{root_node.m_Scale}})
                    .set<Models::Model>({model})
                    .set<Models::BoneTransforms>({std::vector<glm::mat4>(model->m_Bones.size())})
                    .set<Models::NodeTransforms>({std::vector<glm::mat4>(model->m_Nodes.size())})
                    .add<Transform3D::TransformMatrix>()
                    .add<Models::AnimationStack>();
        ;
        if (!separate_meshes)
                root.add<Graphics::RenderModelDirectly>();
        else
        {
                for (const auto& p : model->m_MeshNodePairs)
                {
                        const auto& node = model->m_Nodes[p.m_NodeIndex];
                        w.entity(node.m_Name.c_str())
                        .child_of(root)
                        .add<Transform3D::TransformMatrix>()
                        .add<Transform3D::Translation>()
                        .add<Transform3D::Rotation>()
                        .add<Transform3D::Scale>()
                        .set<Transform3D::LocalTranslation>({{node.m_Pos}})
                        .set<Transform3D::LocalRotation>({{node.m_Rot}})
                        .set<Transform3D::LocalScale>({{node.m_Scale}})
                        .set<Models::ModelNodeID>({p.m_NodeIndex})
                        .set<Models::MeshIndex>({p.m_MeshIndex})
                        .add<Models::CopyNodeModelTransformFromParent>();
                        ;
                }
        }

        return root;
}
}
