#pragma once

#include<drhi.h>

#include"../RenderCamera.h"
#include"../PointLight.h"

namespace FOCUS
{
    typedef struct UniformUpdateData
    {
        Matrix4 model;
        Matrix4 view;
        Matrix4 proj;
        Vector3 lightPosition;
        Vector3 viewPosition;  
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

		virtual void build(std::shared_ptr<DRHI::DynamicRHI> rhi) = 0;
        virtual void updateUniformBuffer(UniformUpdateData uud) = 0;

        void draw(uint32_t index, std::shared_ptr<DRHI::DynamicRHI> rhi)
        {
            auto api = rhi->getCurrentAPI();
            auto bindPoint = DRHI::DynamicPipelineBindPoint(api);

            rhi->bindPipeline(_pipeline, bindPoint.PIPELINE_BIND_POINT_GRAPHICS, index);
            rhi->bindDescriptorSets(&_descriptorSet, _pipelineLayout, 0, index);
        }
	};
}