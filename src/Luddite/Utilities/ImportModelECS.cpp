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
flecs::entity ImportModelECS(Handle<Model> model, flecs::world& w, flecs::entity parent)
{
        std::vector<flecs::entity> entities;
        entities.reserve(model->m_Nodes.size());
        //std::vector<flecs::entity> nodes;
        //for (auto node : model->m_Nodes)

        for (unsigned int i = 0; i < model->m_Nodes.size(); i++)
        {
                const Node& node = model->m_Nodes[i];

                bool is_root = node.m_ParentNodeID == -1;
                auto e = entities.emplace_back(
                        w.entity(node.m_Name.c_str())
                        .child_of(is_root ? parent : entities.at(node.m_ParentNodeID))
                        .add<Transform3D::Translation>()
                        .add<Transform3D::Rotation>()
                        .add<Transform3D::Scale>()
                        .set<Transform3D::LocalTranslation>({{node.m_Pos}})
                        .set<Transform3D::LocalRotation>({{node.m_Rot}})
                        .set<Transform3D::LocalScale>({{node.m_Scale}})
                        //.set<Modles::Model>({Luddite::Assets::GetBasicModelLibrary().GetAsset(4049191577729022337ULL)})
                        );
                if (is_root)
                {
                        LD_VERIFY(i == 0, "Root node of model \"{}\" is not at index 0!", model->m_Name);
                        entities[i].set<Models::Model>({model});
                        if (model->m_Bones.size() > 0)
                        {
                                entities[i].set<Models::BoneTransforms>({std::vector<glm::mat4>(model->m_Bones.size())});
                                entities[i].set<Models::NodeTransforms>({std::vector<glm::mat4>(model->m_Nodes.size())});
                        }
                        entities[i].add<Models::AnimationStack>();
                }
                else
                {
                        entities[i].set<Models::ModelNodeID>({i});
                        //entities[i].add<Models::ChildOfRootNode>(entities[0]);
                }
        }

        //for (const auto& bone : model->m_Bones)
        for (unsigned int i = 0; i < model->m_Bones.size(); i++)
        {
                //entities[model->m_Bones[i].m_NodeID].set<Models::ModelBoneID>({i});
        }
        //entities[0].set<Models::RootNode>({entities});
        return entities[0];
}
}
