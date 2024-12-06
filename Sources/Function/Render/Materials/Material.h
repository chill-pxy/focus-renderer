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
        
        Vector3 pointLightPosition;
        Vector3 pointLightColor;
        float   pointLightIntensity;

        Vector3 dirLightDirection;
        Vector3 dirLightColor;
        float   dirLightStrength;
 
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

        // property
        std::string _name{ "unknown" };
        std::string _ambientTex{ "" };
        std::string _diffuseTex{ "" };
        std::string _specularTex{ "" };

        float _ambient{ 1.0 };
        float _diffuse{ 1.0 };
        float _specular{ 0.0 };
        float _roughness{ 0.0 };
        float _ior{ 0.0 };

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