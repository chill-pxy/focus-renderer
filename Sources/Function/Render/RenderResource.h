#pragma once

#include<drhi.h>

#include<memory>

#include"Materials/Material.h"

namespace FOCUS
{
    class RenderResource
    {
    public:
        DRHI::DynamicBuffer       _vertexBuffer;
        DRHI::DynamicDeviceMemory _vertexDeviceMemory;
        
        DRHI::DynamicBuffer       _indexBuffer;   
        DRHI::DynamicDeviceMemory _indexDeviceMemory;

        std::shared_ptr<Material> _material{};

    public:
        RenderResource() = default;

        virtual void build(std::shared_ptr<DRHI::DynamicRHI> rhi) = 0;
        virtual void draw(uint32_t index, std::shared_ptr<DRHI::DynamicRHI> rhi) = 0;
        virtual void updateUniformBuffer(std::shared_ptr<RenderCamera> camera, std::shared_ptr<PointLight> light) = 0;
    };
}