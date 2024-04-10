//
// Created by pxy on 2024/4/9.
//
module;
#include <memory>
#include <iostream>
module runtime_context;

import window_system;

namespace EAT
{
    void RuntimeContext::startSystems()
    {
        _window_system = std::make_shared<WindowSystem>();
        _window_system->init();
    }
}