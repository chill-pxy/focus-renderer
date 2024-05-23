//
// Created by pxy on 2024/4/9.
//
module;
#include <memory>
#include <iostream>
module runtime_context;

import window_system;
import render_system;

namespace FOCUS
{
    void RuntimeContext::startSystems()
    {
        _window_system = std::make_shared<WindowSystem>();
        _window_system->init();

        _render_system = std::make_shared<RenderSystem>();
        _render_system->init();
    }
}