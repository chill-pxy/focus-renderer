//
// Created by pxy on 2024/4/8.
//
module;
#include<iostream>
#include<memory>
module engine;

import runtime_context;

namespace FOCUS
{
    Engine::Engine()
    {
        _runtime_context = std::make_shared<RuntimeContext>();
    }

    void Engine::startEngine()
    {
        _runtime_context->startSystems();
    }
}

