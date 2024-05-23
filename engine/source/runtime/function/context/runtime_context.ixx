//
// Created by pxy on 2024/4/9.
//
module;

#include <memory>
#include <iostream>

export module runtime_context;

import window_system;
import render_system;

namespace FOCUS
{
    export class RuntimeContext
    {
    public:
        RuntimeContext() = default;
        void startSystems();
        void shutdownSystems();

    public:
        std::shared_ptr<WindowSystem> _window_system;
        std::shared_ptr<RenderSystem> _render_system;
    };
}
