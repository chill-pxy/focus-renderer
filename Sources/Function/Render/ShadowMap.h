#pragma once

#include<drhi.h>

#include"../../Core/Math.h"
#include"Geometry/MeshVertex.h"
#include"Materials/Material.h"

namespace FOCUS
{
	typedef struct ShadowMapUniformBufferObject
	{
		alignas(16) Matrix4 depthMVP;
	}ShadowMapUniformBufferObject;

	class ShadowMap
	{
	public:
		std::shared_ptr<DRHI::DynamicRHI> _rhi;

		DRHI::DynamicPipeline       _shadowPipeline{};
		DRHI::DynamicPipelineLayout _shadowPipelineLayout{};

		DRHI::DynamicDescriptorPool      _descriptorPool{};
		DRHI::DynamicDescriptorSet       _descriptorSet{};
		DRHI::DynamicDescriptorSetLayout _descriptorSetLayout{};

		DRHI::DynamicImage        _depthImage{};
		DRHI::DynamicDeviceMemory _depthImageMemory{};
		DRHI::DynamicImageView    _depthImageView{};
		DRHI::DynamicSampler      _shadowSampler{};

		DRHI::DynamicBuffer       _uniformBuffer{};
		DRHI::DynamicDeviceMemory _uniformBufferMemory{};
		void*                     _uniformBufferMapped{ nullptr };

		DRHI::DynamicDescriptorBufferInfo _descriptorBufferInfo{};

		uint32_t _shadowDepthImageWidth{ 512 };
		uint32_t _shadowDepthImageHeight{ 512 };

		float _zNear = 1.0f;
		float _zFar = 96.0f;

	public:
		ShadowMap() = default;

		void initialize(std::shared_ptr<DRHI::DynamicRHI> rhi)
		{
			_rhi = rhi;

			auto api = _rhi->getCurrentAPI();
			auto format = DRHI::DynamicFormat(api);
			auto tilling = DRHI::DynamicImageTiling(api);
			auto useFlag = DRHI::DynamicImageUsageFlagBits(api);
			auto memoryFlag = DRHI::DynamicMemoryPropertyFlags(api);

			// create Depth image
			_rhi->createImage(&_depthImage, _shadowDepthImageWidth, _shadowDepthImageHeight,
				format.FORMAT_D16_UNORM, tilling.IMAGE_TILING_OPTIMAL, useFlag.IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | useFlag.IMAGE_USAGE_SAMPLED_BIT,
				memoryFlag.MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &_depthImageMemory);

			// create Depth image view
			auto imageAspect = DRHI::DynamicImageAspectFlagBits(api);
			_rhi->createImageView(&_depthImageView, &_depthImage, format.FORMAT_D16_UNORM, imageAspect.IMAGE_ASPECT_DEPTH_BIT);

			// create sampler
			auto borderColor = DRHI::DynamicBorderColor(api);
			auto addressMode = DRHI::DynamicSamplerAddressMode(api);
			DRHI::DynamicSamplerCreateInfo sci{};
			sci.borderColor = borderColor.BORDER_COLOR_FLOAT_OPAQUE_WHITE;
			sci.sampleraAddressMode = addressMode.SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			_rhi->createSampler(&_shadowSampler, sci);

			// create uniform buffer
			_rhi->createUniformBuffer(&_uniformBuffer, &_uniformBufferMemory, &_uniformBufferMapped, sizeof(ShadowMapUniformBufferObject));
			_descriptorBufferInfo.set(_rhi->getCurrentAPI(), _uniformBuffer, sizeof(ShadowMapUniformBufferObject));

			// create descriptor
			auto descriptorType = DRHI::DynamicDescriptorType(api);
			auto stageFlags = DRHI::DynamicShaderStageFlags(api);
			std::vector<DRHI::DynamicDescriptorSetLayoutBinding> dsbs(2);
			dsbs[0].binding = 0;
			dsbs[0].descriptorCount = 1;
			dsbs[0].descriptorType = descriptorType.DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			dsbs[0].pImmutableSamplers = nullptr;
			dsbs[0].stageFlags = stageFlags.SHADER_STAGE_VERTEX_BIT;

			dsbs[1].binding = 1;
			dsbs[1].descriptorCount = 1;
			dsbs[1].descriptorType = descriptorType.DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			dsbs[1].pImmutableSamplers = nullptr;
			dsbs[1].stageFlags = stageFlags.SHADER_STAGE_FRAGMENT_BIT;

			_rhi->createDescriptorSetLayout(&_descriptorSetLayout, &dsbs);

			std::vector<DRHI::DynamicDescriptorPoolSize> poolSizes(2);
			poolSizes[0].type = descriptorType.DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			poolSizes[0].descriptorCount = 3;
			poolSizes[1].type = descriptorType.DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			poolSizes[1].descriptorCount = 3;

			_rhi->createDescriptorPool(&_descriptorPool, &poolSizes);

			std::vector<DRHI::DynamicWriteDescriptorSet> wds(2);
			wds[0].descriptorType = descriptorType.DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			wds[0].dstBinding = 0;
			wds[0].pBufferInfo = &_descriptorBufferInfo;
			wds[0].descriptorCount = 1;

			auto imageLayout = DRHI::DynamicImageLayout(api);
			DRHI::DynamicDescriptorImageInfo dii{};
			dii.imageLayout = imageLayout.IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			dii.imageView = _depthImageView;
			dii.sampler = _shadowSampler;

			wds[1].descriptorType = descriptorType.DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			wds[1].dstBinding = 1;
			wds[1].descriptorCount = 1;
			wds[1].pImageInfo = &dii;

			_rhi->createDescriptorSet(&_descriptorSet, &_descriptorSetLayout, &_descriptorPool, &wds);

			// create pipeline
			DRHI::DynamicPipelineCreateInfo pci = {};
			pci.vertexShader = "../../../Shaders/shadowMapVertex.spv";
			pci.vertexInputBinding = DRHI::DynamicVertexInputBindingDescription();
			pci.vertexInputBinding.set(api, 0, sizeof(Vertex));
			pci.vertexInputAttributes = std::vector<DRHI::DynamicVertexInputAttributeDescription>();
			pci.vertexInputAttributes.resize(1);
			pci.vertexInputAttributes[0].set(api, 0, 0, format.FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Vertex::pos));

			DRHI::DynamicPipelineLayoutCreateInfo plci{};
			plci.pSetLayouts = &_descriptorSetLayout;
			plci.setLayoutCount = 1;
			plci.pushConstantRangeCount = 0;

			_rhi->createPipelineLayout(&_shadowPipelineLayout, &plci);

			_rhi->createPipeline(&_shadowPipeline, &_shadowPipelineLayout, pci);
		}

		void updateUniform(UniformUpdateData ubo)
		{
			// Matrix from light's point of view
			Matrix4 depthProjectionMatrix = perspective(radians(45.0f), 1.0f, _zNear, _zFar);
			Matrix4 depthViewMatrix = lookAt(ubo.pointLightPosition, Vector3(0.0f), Vector3(0, 1, 0));
			Matrix4 depthModelMatrix = Matrix4(1.0f);

			ShadowMapUniformBufferObject subo{};
			subo.depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;

			memcpy(_uniformBufferMapped, &subo, sizeof(ShadowMapUniformBufferObject));
		}
	};


}