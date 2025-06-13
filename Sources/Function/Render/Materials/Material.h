#pragma once

#include<drhi.h>

#include"../../../Core/Path.h"
#include"../../../Core/Math.h"
#include"../Texture.h"

namespace focus
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

    typedef struct GBuffer
    {
        drhi::DynamicImageView* albedoImageView{ nullptr };
        drhi::DynamicSampler*   albedoSampler{ nullptr };
        drhi::DynamicImageView* positionImageView{ nullptr };
        drhi::DynamicSampler*   positionSampler{ nullptr };
        drhi::DynamicImageView* normalImageView{ nullptr };
        drhi::DynamicSampler*   normalSampler{ nullptr };
        drhi::DynamicImageView* depthImageView{ nullptr };
        drhi::DynamicSampler*   depthSampler{ nullptr };
    }GBuffer;

	class Material
	{
	public:
        std::string _type = "Material Base";

        // resources
        std::shared_ptr<Texture> _basicTexture;

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
        bool _defferdDraw{ false };

        // rhi resources
        GBuffer _gbuffer{};

        drhi::DynamicImage        _textureImage{};
        drhi::DynamicImageView    _textureImageView{};
        drhi::DynamicSampler      _textureSampler{};
        drhi::DynamicDeviceMemory _textureMemory{};

        drhi::DynamicImageView* _shadowImageView{ nullptr };
        drhi::DynamicSampler*   _shadowSampler{ nullptr };

        drhi::DynamicImageView* _brdfImageView{ nullptr };
        drhi::DynamicSampler*   _brdfSampler{ nullptr };

        drhi::DynamicImageView* _irradianceImageView{ nullptr };
        drhi::DynamicSampler*   _irradianceSampler{ nullptr };

        drhi::DynamicImageView* _filteredImageView{ nullptr };
        drhi::DynamicSampler*   _filteredImageSampler{ nullptr };

        drhi::DynamicDescriptorPool      _descriptorPool{};
        drhi::DynamicDescriptorSet       _descriptorSet{};
        drhi::DynamicDescriptorSetLayout _descriptorSetLayout{};

        drhi::DynamicPipeline       _pipeline{};
        drhi::DynamicPipelineLayout _pipelineLayout{};


	public:
		Material() = default;

		virtual void build(std::shared_ptr<drhi::DynamicRHI> rhi) = 0;
        virtual void updateUniformBuffer(UniformUpdateData uud) = 0;
        virtual void clean(std::shared_ptr<drhi::DynamicRHI> rhi) = 0;

        void draw(std::shared_ptr<drhi::DynamicRHI> rhi, drhi::DynamicCommandBuffer* commandBuffer)
        {
            auto api = rhi->getCurrentAPI();
            auto bindPoint = drhi::DynamicPipelineBindPoint(api);

            rhi->bindPipeline(_pipeline, commandBuffer, bindPoint.PIPELINE_BIND_POINT_GRAPHICS);
            rhi->bindDescriptorSets(&_descriptorSet, _pipelineLayout, commandBuffer, bindPoint.PIPELINE_BIND_POINT_GRAPHICS);
        }

        virtual void buildTexture(std::shared_ptr<drhi::DynamicRHI> rhi, drhi::DynamicCommandPool* commandPool)
        {
            //binding sampler and image view
            auto api = rhi->getCurrentAPI();
            auto format = drhi::DynamicFormat(api);
            auto imageAspect = drhi::DynamicImageAspectFlagBits(api);
            rhi->createTextureImage(&_textureImage, &_textureMemory, commandPool, _basicTexture->_width, _basicTexture->_height, _basicTexture->_channels, _basicTexture->_pixels);
            rhi->createImageView(&_textureImageView, &_textureImage, format.FORMAT_R8G8B8A8_SRGB, imageAspect.IMAGE_ASPECT_COLOR_BIT);
            rhi->createTextureSampler(&_textureSampler);
        }
	};
}