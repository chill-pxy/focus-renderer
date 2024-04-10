//
// Created by pxy on 2024/4/10.
//

export module render_system;

namespace EAT
{
    export class RenderSystem
    {
    public:
        RenderSystem() = default;
        ~RenderSystem();

        void init();
        void tick();
    };
}