#pragma once

#include<drhi.h>

#include<memory>

#include"Materials/Material.h"
#include"ShadowMap.h"
#include"DefferedPipeline.h"

namespace focus
{
    enum RenderResourcePipeline
    {
        SCENE,
        SHADOW,
        DEFFERED
    };

    class RenderResource
    {
    public:
        std::string _name{ "unnamed object" };

        drhi::DynamicBuffer       _vertexBuffer;
        drhi::DynamicDeviceMemory _vertexDeviceMemory;
        
        drhi::DynamicBuffer       _indexBuffer;   
        drhi::DynamicDeviceMemory _indexDeviceMemory;

        std::shared_ptr<Material> _material{};
        std::shared_ptr<ShadowMap> _shadow{};
        std::shared_ptr<DefferedPipeline> _deffered{};

        bool _isLightActor{ false };
        bool _castShadow{ true };
        bool _built{ false };

        Vector3 _position{ 0.0,0.0,0.0 };
        Vector3 _color{ 1.0f, 1.0f, 1.0f };
        Vector3 _scale{ 1.0f, 1.0f, 1.0f };
        Vector3 _rotate{ 0.0f, 0.0f, 0.0f };
        Matrix4 _rotation{1.0f};

    public:
        RenderResource() = default;

        virtual void build(std::shared_ptr<drhi::DynamicRHI> rhi, drhi::DynamicCommandPool* commandPool) = 0;
        virtual void draw(std::shared_ptr<drhi::DynamicRHI> rhi, drhi::DynamicCommandBuffer* commandBuffer, RenderResourcePipeline pipeline) = 0;
        virtual void draw(std::shared_ptr<drhi::DynamicRHI> rhi, drhi::DynamicCommandBuffer* commandBuffer, drhi::DynamicPipeline pipeline, drhi::DynamicPipelineLayout pipelineLayout, drhi::DynamicDescriptorSet set) = 0;
        virtual void updateUniformBuffer(UniformUpdateData uud) = 0;
        virtual void buildPipeline(std::shared_ptr<drhi::DynamicRHI> rhi, drhi::DynamicCommandPool* commandPool)
        {
            //initiailize shadow map
            _shadow = std::make_shared<ShadowMap>();
            _shadow->initialize(rhi);

            if (_material->_basicTexture) _material->buildTexture(rhi, commandPool);

            //initialize deffered
            if (!_isLightActor)
            {
                _deffered = std::make_shared<DefferedPipeline>();
                _deffered->_textureImageView = _material->_textureImageView;
                _deffered->_textureSampler = _material->_textureSampler;
                _deffered->initialize(rhi);
            }

            // build material
            _material->build(rhi);
        }
    };
}