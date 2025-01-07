#pragma once

#include<chrono>
#include<thread>

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
	private:
		double _angleIncrement = PI / 180.0 / 10;
		double _theta = 0.0f;
		int _count = 0;

	public:
		std::shared_ptr<DRHI::DynamicRHI> _rhi;

		DRHI::DynamicPipeline       _shadowPipeline{};
		DRHI::DynamicPipelineLayout _shadowPipelineLayout{};

		DRHI::DynamicDescriptorPool      _descriptorPool{};
		DRHI::DynamicDescriptorSet       _descriptorSet{};
		DRHI::DynamicDescriptorSetLayout _descriptorSetLayout{};

		DRHI::DynamicBuffer       _uniformBuffer{};
		DRHI::DynamicDeviceMemory _uniformBufferMemory{};
		void*                     _uniformBufferMapped{ nullptr };

		DRHI::DynamicDescriptorBufferInfo _descriptorBufferInfo{};

	public:
		ShadowMap() = default;

		void initialize(std::shared_ptr<DRHI::DynamicRHI> rhi, DRHI::DynamicCommandPool* commandPool)
		{
			_rhi = rhi;

			auto api = _rhi->getCurrentAPI();
			auto format = DRHI::DynamicFormat(api);

			// create uniform buffer
			_rhi->createUniformBuffer(&_uniformBuffer, &_uniformBufferMemory, &_uniformBufferMapped, sizeof(ShadowMapUniformBufferObject));
			_descriptorBufferInfo.set(_rhi->getCurrentAPI(), _uniformBuffer, sizeof(ShadowMapUniformBufferObject));

			// create descriptor
			auto descriptorType = DRHI::DynamicDescriptorType(api);
			auto stageFlags = DRHI::DynamicShaderStageFlags(api);
			std::vector<DRHI::DynamicDescriptorSetLayoutBinding> dsbs(1);
			dsbs[0].binding = 0;
			dsbs[0].descriptorCount = 1;
			dsbs[0].descriptorType = descriptorType.DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			dsbs[0].pImmutableSamplers = nullptr;
			dsbs[0].stageFlags = stageFlags.SHADER_STAGE_VERTEX_BIT;

			_rhi->createDescriptorSetLayout(&_descriptorSetLayout, &dsbs);

			std::vector<DRHI::DynamicDescriptorPoolSize> poolSizes(1);
			poolSizes[0].type = descriptorType.DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			poolSizes[0].descriptorCount = 3;

			_rhi->createDescriptorPool(&_descriptorPool, &poolSizes);

			std::vector<DRHI::DynamicWriteDescriptorSet> wds(1);
			wds[0].descriptorType = descriptorType.DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			wds[0].dstBinding = 0;
			wds[0].pBufferInfo = &_descriptorBufferInfo;
			wds[0].descriptorCount = 1;

			_rhi->createDescriptorSet(&_descriptorSet, &_descriptorSetLayout, &_descriptorPool, &wds, 0);

			// create pipeline
			DRHI::DynamicPipelineCreateInfo pci = {};
			pci.vertexShader = "../../../Shaders/shadowMapVertex.spv";
			pci.vertexInputBinding = DRHI::DynamicVertexInputBindingDescription();
			pci.vertexInputBinding.set(api, 0, sizeof(Vertex));
			pci.vertexInputAttributes = std::vector<DRHI::DynamicVertexInputAttributeDescription>();
			pci.vertexInputAttributes.resize(2);
			pci.vertexInputAttributes[0].set(api, 0, 0, format.FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Vertex::pos));
			pci.vertexInputAttributes[1].set(api, 1, 0, format.FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Vertex::color));
			pci.colorImageFormat = format.FORMAT_UNDEFINED;
			pci.depthImageFormat = format.FORMAT_D16_UNORM;
			pci.includeStencil = false;
			pci.dynamicDepthBias = true;

			DRHI::DynamicPipelineLayoutCreateInfo plci{};
			plci.pSetLayouts = &_descriptorSetLayout;
			plci.setLayoutCount = 1;
			plci.pushConstantRangeCount = 0;

			_rhi->createPipelineLayout(&_shadowPipelineLayout, &plci);
			
			_rhi->createPipeline(&_shadowPipeline, &_shadowPipelineLayout, pci);
		}

		void updateUniform(UniformUpdateData& ubo)
		{
			_count++;
			if (_count >= 3600)
			{
				_count = 0;
				_theta = 0.0;
			}

			Vector3 lightPosition = Vector3(
			    500.0f * cos(_theta),
				1000, 
				500.0f * sin(_theta) 
			);

			_theta += _angleIncrement;

			// Matrix from light's point of view
			Matrix4 depthProjectionMatrix = perspective(radians(45.0f), 1.0f, 100.0f, 2000.0f);
			Matrix4 depthViewMatrix = lookAt(lightPosition, Vector3(0.0f), Vector3(0, -1, 0));
			Matrix4 depthModelMatrix = ubo.model;

			ShadowMapUniformBufferObject subo{};
			subo.depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;
			ubo.dirLightSpace = subo.depthMVP;

			memcpy(_uniformBufferMapped, &subo, sizeof(ShadowMapUniformBufferObject));
		}

		void draw(std::shared_ptr<DRHI::DynamicRHI> rhi, DRHI::DynamicCommandBuffer* commandBuffer)
		{
			auto api = _rhi->getCurrentAPI();
			auto bindPoint = DRHI::DynamicPipelineBindPoint(api);

			_rhi->cmdSetDepthBias(*commandBuffer, 1.8f, 0.0f, 2.0f);
			_rhi->bindPipeline(_shadowPipeline, commandBuffer, bindPoint.PIPELINE_BIND_POINT_GRAPHICS);
			_rhi->bindDescriptorSets(&_descriptorSet, _shadowPipelineLayout, commandBuffer, bindPoint.PIPELINE_BIND_POINT_GRAPHICS);
		}

		void clean(std::shared_ptr<DRHI::DynamicRHI> rhi)
		{
			rhi->clearBuffer(&_uniformBuffer, &_uniformBufferMemory);

			rhi->freeDescriptorSets(&_descriptorSet, &_descriptorPool);
			rhi->clearDescriptorPool(&_descriptorPool);
			rhi->clearDescriptorSetLayout(&_descriptorSetLayout);


			rhi->clearPipeline(&_shadowPipeline, &_shadowPipelineLayout);
		}
	};


}