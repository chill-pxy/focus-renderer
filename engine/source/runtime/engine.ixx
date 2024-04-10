//
// Created by pxy on 2024/4/8.
//
module;
#include<memory>
export module engine;

import runtime_context;

namespace EAT
{
    export class Engine
    {
    public:
        Engine();

        void startEngine();
    
    private:
        std::shared_ptr<RuntimeContext> _runtime_context;
        
    };
}