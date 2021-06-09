#include "AppLayer.hpp"
#include "Components.hpp"

class TestSystem : public Luddite::System<TestSystem>
{
        public:
        void Configure()
        {
                // RegisterView<C_Transform>()
                // RegisterGroup<C_Transform>();
        }

        void Update(Luddite::World& world, float delta_time)
        {
                for (const auto &&[id, t, t2]: world.GetGroup<C_Test>(Luddite::Borrow<C_Test2>).each())
                {
                        auto Entity = world.GetEntityFromID(id);
                        Entity.ReplaceComponent<C_Test>(120.30f);
                }
        }
};

void AppLayer::Initialize()
{
        m_World.RegisterSystem<TestSystem>();
        m_World.RegisterSystem<S_Render>();
        m_World.ConfigureSystems();

        auto e = m_World.CreateEntity();
        e.AddComponent<C_Transform3D>();
        e.AddComponent<C_Model>(Luddite::ModelLoader::GetBasicModel("Assets/suzanne.obj"));


        auto camera = m_World.CreateEntity();
        camera.AddComponent<C_Transform3D>();
        camera.AddComponent<C_Camera>();
        m_World.SetSingleton<C_ActiveCamera>(camera.GetID());
}

void AppLayer::HandleEvents()
{
}

void AppLayer::Update(double delta_time)
{
        // m_World.UpdateSystem<TestSystem>(m_World, delta_time);
}

void AppLayer::Render(double alpha, Luddite::RenderTarget render_target)
{
        m_World.UpdateSystem<S_Render>(m_World, alpha, render_target);
}