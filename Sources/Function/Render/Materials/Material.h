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

        Vector3 dirLightPosition;
        Vector3 dirLightColor;
        Vector3 dirLightDirection;
        float   dirLightStrength;
        Matrix4 dirLightSpace;

        float ambient;
        float diffuse;
        float specular;
        float roughness;
        float shinness;
        float metallic;
 
    }UniformUpdateData;

	class Material
	{
	public:
        DRHI::DynamicImage        _textureImage{};
        DRHI::DynamicImageView    _textureImageView{};
        DRHI::DynamicSampler      _textureSampler{};
        DRHI::DynamicDeviceMemory _textureMemory{};

        DRHI::DynamicImageView* _brdfImageView{ nullptr };
        DRHI::DynamicSampler* _brdfSampler{ nullptr };

        DRHI::DynamicImageView*  _irradianceImageView{nullptr};
        DRHI::DynamicSampler*    _irradianceSampler{nullptr};

        DRHI::DynamicImageView*    _filteredImageView{nullptr};
        DRHI::DynamicSampler*      _filteredImageSampler{nullptr};

        DRHI::DynamicDescriptorPool      _descriptorPool{};
        DRHI::DynamicDescriptorSet       _descriptorSet{};
        DRHI::DynamicDescriptorSetLayout _descriptorSetLayout{};

        DRHI::DynamicPipeline       _pipeline{};
        DRHI::DynamicPipelineLayout _pipelineLayout{};

        std::string _type = "Material Base";

        // property
        std::string _name{ "unknown" };
        std::string _ambientTex{ "" };
        std::string _diffuseTex{ "" };
        std::string _specularTex{ "" };

        uint32_t _cullMode = { 0 };

        float _ambient{ 1.0 };
        float _diffuse{ 1.0 };
        float _specular{ 0.0 };
        float _roughness{ 0.0 };
        float _shinness{ 0.0 };
        float _ior{ 0.0 };
        float _metallic{ 0.0 };

        uint32_t _mipLevel{ 0 };

        bool _built{ false };
        bool _cleared{ false };

	public:
		Material() = default;

		virtual void build(std::shared_ptr<DRHI::DynamicRHI> rhi, DRHI::DynamicCommandPool* commandPool, DRHI::DynamicImage shadowImage, DRHI::DynamicImageView shadowImageView, DRHI::DynamicSampler shadowSampler) = 0;
        virtual void updateUniformBuffer(UniformUpdateData uud) = 0;
        virtual void clean(std::shared_ptr<DRHI::DynamicRHI> rhi) = 0;

        void draw(std::shared_ptr<DRHI::DynamicRHI> rhi, DRHI::DynamicCommandBuffer* commandBuffer)
        {
            auto api = rhi->getCurrentAPI();
            auto bindPoint = DRHI::DynamicPipelineBindPoint(api);

            rhi->bindPipeline(_pipeline, commandBuffer, bindPoint.PIPELINE_BIND_POINT_GRAPHICS);
            rhi->bindDescriptorSets(&_descriptorSet, _pipelineLayout, commandBuffer, bindPoint.PIPELINE_BIND_POINT_GRAPHICS);
        }
	};
}