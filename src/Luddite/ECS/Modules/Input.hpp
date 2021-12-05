#pragma once
#include "Luddite/Core/Event.hpp"
#include "Luddite/Core/EventPool.hpp"
#include "Luddite/Core/Logging.hpp"
#include "Luddite/Core/pch.hpp"
#include "Luddite/ECS/Modules/LudditeBase.hpp"
#include "Luddite/ECS/Reflection.hpp"
#include "Luddite/Platform/IO/IO.hpp"

namespace Input
{
LD_COMPONENT_DEFINE(KeyboardState,
        (),
        ((std::array<bool, Luddite::IO::GetNumKeys()>) KeyDown, ({false})),
        ((std::array<bool, Luddite::IO::GetNumKeys()>) KeyPressed, ({false})),
        ((std::array<bool, Luddite::IO::GetNumKeys()>) KeyReleased, ({false}))
        )

LD_COMPONENT_DEFINE(MouseState,
        (),
        ((std::array<bool, 32>) ButtonDown, ({false})),
        ((std::array<bool, 32>) ButtonPressed, ({false})),
        ((std::array<bool, 32>) ButtonReleased, ({false})),
        ((glm::vec2)Position, ({})),
        ((glm::vec2)Delta, ({}))
        )
struct Components
{
        Components(flecs::world& w)
        {
                w.module<Components>();
                LD_COMPONENT_REGISTER(KeyboardState, "Keyboard State", w);
                w.set<KeyboardState>({});
        }
};
struct Systems
{
        Systems(flecs::world& w)
        {
                w.import<Luddite::Components>();
                w.import<Components>();
                w.module<Systems>();
                w.system<>("Update Keyboard State")
                .kind(w.id<Luddite::OnInput>())
                .term<KeyboardState>().inout(flecs::Out).singleton()
                .iter([](flecs::iter it){
                                auto ks = it.term<KeyboardState>(1);
                                std::fill(ks->KeyPressed.begin(), ks->KeyPressed.end(), false);
                                std::fill(ks->KeyReleased.begin(), ks->KeyReleased.end(), false);
                                for (auto e : Luddite::EventList<Luddite::KeyPressEvent>())
                                {
                                        ks->KeyPressed[static_cast<uint32_t>(e.key_code)] = true;
                                        ks->KeyDown[static_cast<uint32_t>(e.key_code)] = true;
                                }
                                for (auto e : Luddite::EventList<Luddite::KeyReleaseEvent>())
                                {
                                        ks->KeyReleased[static_cast<uint32_t>(e.key_code)] = true;
                                        ks->KeyDown[static_cast<uint32_t>(e.key_code)] = false;
                                }
                        });
                w.system<>("Update Mouse State")
                .kind(w.id<Luddite::OnInput>())
                .term<MouseState>().inout(flecs::Out).singleton()
                .iter([](flecs::iter it){
                                auto ms = it.term<MouseState>(1);
                                std::fill(ms->ButtonPressed.begin(), ms->ButtonPressed.end(), false);
                                std::fill(ms->ButtonReleased.begin(), ms->ButtonReleased.end(), false);
                                for (auto e : Luddite::EventList<Luddite::MouseButtonPressEvent>())
                                {
                                        ms->ButtonPressed[static_cast<uint32_t>(e.button)] = true;
                                        ms->ButtonDown[static_cast<uint32_t>(e.button)] = true;
                                }
                                for (auto e : Luddite::EventList<Luddite::MouseButtonReleaseEvent>())
                                {
                                        ms->ButtonReleased[static_cast<uint32_t>(e.button)] = true;
                                        ms->ButtonDown[static_cast<uint32_t>(e.button)] = false;
                                }
                                ms->Delta = glm::vec2{0.f, 0.f};
                                for (auto e : Luddite::EventList<Luddite::MouseMotionEvent>())
                                {
                                        glm::vec2 new_pos{e.x, e.y};
                                        ms->Delta += new_pos - ms->Position;
                                        ms->Position = new_pos;
                                }
                        });
        }
};
}
