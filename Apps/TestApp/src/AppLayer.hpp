#include "Luddite/Luddite.hpp"
class AppLayer : public Luddite::Layer
{
        void Initialize();
        void HandleEvents();
        void Update(double delta_time);
        void Render(double alpha);
};