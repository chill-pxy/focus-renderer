#pragma once

#include<chrono>
#include<thread>

#include<drhi.h>

#include"../../Core/Math.h"
#include"Geometry/MeshVertex.h"
#include"Materials/Material.h"

namespace focus
{
	typedef struct ShadowMapUniformBufferObject
	{
		alignas(16) Matrix4 depthMVP;
	}ShadowMapUniformBufferObject;

	class ShadowMap
	{
	private:
		double _angleIncrement = PI / 180.0 / 10;
		double _theta = 0.0f;
		int _count = 0;

	public:
		drhi::DynamicPipeline       _shadowPipeline{};
		drhi::DynamicPipelineLayout _shadowPipelineLayout{};

		drhi::DynamicDescriptorPool      _descriptorPool{};
		drhi::DynamicDescriptorSet       _descriptorSet{};
		drhi::DynamicDescriptorSetLayout _descriptorSetLayout{};

		drhi::DynamicBuffer       _uniformBuffer{};
		drhi::DynamicDeviceMemory _uniformBufferMemory{};
		void*                     _uniformBufferMapped{ nullptr };

		drhi::DynamicDescriptorBufferInfo _descriptorBufferInfo{};

	public:
		ShadowMap() = default;

		void initialize(std::shared_ptr<drhi::DynamicRHI> rhi)
		{
			auto api = rhi->getCurrentAPI();
			auto format = drhi::DynamicFormat(api);
			auto sampleCount = drhi::DynamicSampleCountFlags(api);

			// create uniform buffer
			rhi->createUniformBuffer(&_uniformBuffer, &_uniformBufferMemory, &_uniformBufferMapped, sizeof(ShadowMapUniformBufferObject));
			_descriptorBufferInfo.set(rhi->getCurrentAPI(), _uniformBuffer, sizeof(ShadowMapUniformBufferObject));

			// create descriptor
			auto descriptorType = drhi::DynamicDescriptorType(api);
			auto stageFlags = drhi::DynamicShaderStageFlags(api);
			std::vector<drhi::DynamicDescriptorSetLayoutBinding> dsbs(1);
			dsbs[0].binding = 0;
			dsbs[0].descriptorCount = 1;
			dsbs[0].descriptorType = descriptorType.DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			dsbs[0].pImmutableSamplers = nullptr;
			dsbs[0].stageFlags = stageFlags.SHADER_STAGE_VERTEX_BIT;

			rhi->createDescriptorSetLayout(&_descriptorSetLayout, &dsbs);

			std::vector<drhi::DynamicDescriptorPoolSize> poolSizes(1);
			poolSizes[0].type = descriptorType.DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			poolSizes[0].descriptorCount = 3;

			rhi->createDescriptorPool(&_descriptorPool, &poolSizes);

			std::vector<drhi::DynamicWriteDescriptorSet> wds(1);
			wds[0].descriptorType = descriptorType.DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			wds[0].dstBinding = 0;
			wds[0].pBufferInfo = &_descriptorBufferInfo;
			wds[0].descriptorCount = 1;

			rhi->createDescriptorSet(&_descriptorSet, &_descriptorSetLayout, &_descriptorPool, &wds, 0);

			// create pipeline
			drhi::DynamicPipelineCreateInfo pci = {};
			pci.vertexShader = RESOURCE_PATH"Shaders/Shadow/shadowMapVertex.spv";
			pci.vertexInputBinding = drhi::DynamicVertexInputBindingDescription();
			pci.vertexInputBinding.set(api, 0, sizeof(Vertex));
			pci.vertexInputAttributes = std::vector<drhi::DynamicVertexInputAttributeDescription>();
			pci.vertexInputAttributes.resize(2);
			pci.vertexInputAttributes[0].set(api, 0, 0, format.FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Vertex::pos));
			pci.vertexInputAttributes[1].set(api, 1, 0, format.FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Vertex::color));
			pci.colorImageFormat = format.FORMAT_UNDEFINED;
			pci.depthImageFormat = format.FORMAT_D16_UNORM;
			pci.includeStencil = false;
			pci.dynamicDepthBias = true;
			pci.sampleCounts = sampleCount.SAMPLE_COUNT_1_BIT;

			drhi::DynamicPipelineLayoutCreateInfo plci{};
			plci.pSetLayouts = &_descriptorSetLayout;
			plci.setLayoutCount = 1;
			plci.pushConstantRangeCount = 0;

			rhi->createPipelineLayout(&_shadowPipelineLayout, &plci);
			
			rhi->createPipeline(&_shadowPipeline, &_shadowPipelineLayout, pci);
		}

		void updateUniform(UniformUpdateData& ubo)
		{
			// Matrix from light's point of view
			Matrix4 depthProjectionMatrix = ortho(-500.0f, 500.0f, -500.0f, 500.0f, 10.0f, 1000.0f);
			//Matrix4 depthProjectionMatrix = perspective(radians(45.0f), 1.0f, 300.0f, 1200.0f);
			Matrix4 depthViewMatrix = lookAt(ubo.dirLightPosition, Vector3(0.0f), Vector3(0, 1, 0));
			Matrix4 depthModelMatrix = ubo.model;

			ShadowMapUniformBufferObject subo{};
			subo.depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;
			ubo.dirLightSpace = subo.depthMVP;

			memcpy(_uniformBufferMapped, &subo, sizeof(ShadowMapUniformBufferObject));
		}

		void draw(std::shared_ptr<drhi::DynamicRHI> rhi, drhi::DynamicCommandBuffer* commandBuffer)
		{
			auto api = rhi->getCurrentAPI();
			auto bindPoint = drhi::DynamicPipelineBindPoint(api);

			rhi->cmdSetDepthBias(*commandBuffer, 0.5f, 0.0f, 2.0f);
			rhi->bindPipeline(_shadowPipeline, commandBuffer, bindPoint.PIPELINE_BIND_POINT_GRAPHICS);
			rhi->bindDescriptorSets(&_descriptorSet, _shadowPipelineLayout, commandBuffer, bindPoint.PIPELINE_BIND_POINT_GRAPHICS);
		}

		void clean(std::shared_ptr<drhi::DynamicRHI> rhi)
		{
			rhi->clearBuffer(&_uniformBuffer, &_uniformBufferMemory);

			rhi->freeDescriptorSets(&_descriptorSet, &_descriptorPool);
			rhi->clearDescriptorPool(&_descriptorPool);
			rhi->clearDescriptorSetLayout(&_descriptorSetLayout);


			rhi->clearPipeline(&_shadowPipeline, &_shadowPipelineLayout);
		}
	};


}