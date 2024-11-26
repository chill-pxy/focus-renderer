#pragma once

#include<drhi.h>

#include"../../../Core/Math.h"

namespace FOCUS
{
    typedef struct UniformUpdateData
    {
        Matrix4 model;
        Matrix4 view;
        Matrix4 proj;

        Vector3 viewPosition;  
        
        Vector3 vertexColor;
        
        Vector3 lightPosition;
        Vector3 lightColor;
        float lightIntensity;
 
    }UniformUpdateData;

	class Material
	{
	public:
        DRHI::DynamicImage        _textureImage{};
        DRHI::DynamicImageView    _textureImageView{};
        DRHI::DynamicSampler      _textureSampler{};
        DRHI::DynamicDeviceMemory _textureMemory{};

        DRHI::DynamicDescriptorPool                    _descriptorPool{};
        DRHI::DynamicDescriptorSet                     _descriptorSet{};
        DRHI::DynamicDescriptorSetLayout               _descriptorSetLayout{};

        DRHI::DynamicPipeline       _pipeline{};
        DRHI::DynamicPipelineLayout _pipelineLayout{};

	public:
		Material() = default;

		virtual void build(std::shared_ptr<DRHI::DynamicRHI> rhi, DRHI::DynamicCommandPool* commandPool) = 0;
        virtual void updateUniformBuffer(UniformUpdateData uud) = 0;
        virtual void clean(std::shared_ptr<DRHI::DynamicRHI> rhi) = 0;

        void draw(std::shared_ptr<DRHI::DynamicRHI> rhi, DRHI::DynamicCommandBuffer* commandBuffer)
        {
            auto api = rhi->getCurrentAPI();
            auto bindPoint = DRHI::DynamicPipelineBindPoint(api);

            rhi->bindPipeline(_pipeline, commandBuffer, bindPoint.PIPELINE_BIND_POINT_GRAPHICS);
            rhi->bindDescriptorSets(&_descriptorSet, _pipelineLayout, commandBuffer, 0);
        }
	};
}