#pragma once

#include<drhi.h>

#include"../../../Core/Math.h"
#include"../RenderCamera.h"

namespace FOCUS
{
	class Material
	{
	public:
        DRHI::DynamicImage        _textureImage;
        DRHI::DynamicImageView    _textureImageView;
        DRHI::DynamicSampler      _textureSampler;
        DRHI::DynamicDeviceMemory _textureMemory;

        DRHI::DynamicDescriptorPool                    _descriptorPool;
        DRHI::DynamicDescriptorSet                     _descriptorSet;
        DRHI::DynamicDescriptorSetLayout               _descriptorSetLayout;

        DRHI::DynamicPipeline       _pipeline;
        DRHI::DynamicPipelineLayout _pipelineLayout;

	public:
		Material() = default;

		virtual void build(std::shared_ptr<DRHI::DynamicRHI> rhi) = 0;
        virtual void updateUniformBuffer(uint32_t currentImage, std::shared_ptr<RenderCamera> camera) = 0;

        void draw(uint32_t index, std::shared_ptr<DRHI::DynamicRHI> rhi)
        {
            auto api = rhi->getCurrentAPI();
            auto bindPoint = DRHI::DynamicPipelineBindPoint(api);

            rhi->bindPipeline(_pipeline, bindPoint.PIPELINE_BIND_POINT_GRAPHICS, index);
            rhi->bindDescriptorSets(&_descriptorSet, _pipelineLayout, 0, index);
        }
	};
}