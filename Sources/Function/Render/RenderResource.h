#pragma once

#include<drhi.h>

#include<memory>

#include"Materials/Material.h"
#include"ShadowMap.h"

namespace FOCUS
{
    class RenderResource
    {
    public:
        std::string _name{ "unnamed object" };

        DRHI::DynamicBuffer       _vertexBuffer;
        DRHI::DynamicDeviceMemory _vertexDeviceMemory;
        
        DRHI::DynamicBuffer       _indexBuffer;   
        DRHI::DynamicDeviceMemory _indexDeviceMemory;

        std::shared_ptr<Material> _material{};
        std::shared_ptr<ShadowMap> _shadow{};

        Vector3 _position{ 0.0,0.0,0.0 };
        Vector3 _color{ 1.0f, 1.0f, 1.0f };
        Vector3 _scale{ 1.0f, 1.0f, 1.0f };

    public:
        RenderResource() = default;

        virtual void build(std::shared_ptr<DRHI::DynamicRHI> rhi, DRHI::DynamicCommandPool* commandPool, DRHI::DynamicImage shadowImage, DRHI::DynamicImageView shadowImageView, DRHI::DynamicSampler shadowSampler) = 0;
        virtual void draw(std::shared_ptr<DRHI::DynamicRHI> rhi, DRHI::DynamicCommandBuffer* commandBuffer, bool usingMaterial) = 0;
        virtual void updateUniformBuffer(UniformUpdateData uud) = 0;
    };
}