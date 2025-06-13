#pragma once

namespace focus
{
	class Pipeline
	{
    public:
        bool _isCube{ false };

        drhi::DynamicDescriptorSetLayout _descriptorSetLayout{};
        drhi::DynamicDescriptorPool _descriptorPool{};
        drhi::DynamicDescriptorSet _descriptorSet{};
        drhi::DynamicPipeline _pipeline{};
        drhi::DynamicPipelineLayout _pipelineLayout{};

        drhi::DynamicImageView    _textureImageView{};
        drhi::DynamicSampler      _textureSampler{};

        void* _uniformBufferMapped{ nullptr };
        drhi::DynamicBuffer               _uniformBuffer;
        drhi::DynamicDeviceMemory         _uniformBufferMemory;
        drhi::DynamicDescriptorBufferInfo _descriptorBufferInfo;

        virtual void clean(std::shared_ptr<drhi::DynamicRHI> rhi)
        {
            rhi->clearBuffer(&_uniformBuffer, &_uniformBufferMemory);

            rhi->freeDescriptorSets(&_descriptorSet, &_descriptorPool);
            rhi->clearDescriptorPool(&_descriptorPool);
            rhi->clearDescriptorSetLayout(&_descriptorSetLayout);

            rhi->clearPipeline(&_pipeline, &_pipelineLayout);
        }

        void draw(std::shared_ptr<drhi::DynamicRHI> rhi, drhi::DynamicCommandBuffer* commandBuffer)
        {
            auto api = rhi->getCurrentAPI();
            auto bindPoint = drhi::DynamicPipelineBindPoint(api);

            rhi->bindPipeline(_pipeline, commandBuffer, bindPoint.PIPELINE_BIND_POINT_GRAPHICS);
            rhi->bindDescriptorSets(&_descriptorSet, _pipelineLayout, commandBuffer, bindPoint.PIPELINE_BIND_POINT_GRAPHICS);
        }
	};
}