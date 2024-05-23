//
// Created by pxy on 2024/4/10.
//

module;
#include<iostream>
module render_system;

import vulkan_rhi;

namespace FOCUS
{
    void RenderSystem::init()
    {
        auto rhi = new VulkanRHI();
        rhi->init();
    }
}