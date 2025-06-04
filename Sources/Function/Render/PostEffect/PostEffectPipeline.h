#pragma once

#include<drhi.h>

#include"../../Core/Path.h"
#include"../../Core/Math.h"

namespace focus
{
	typedef struct PostEffectPipelineUniformBuffer
	{
        Matrix4 LastVP;
	};

	class PostEffectPipeline
	{
	public:
		drhi::DynamicDescriptorSetLayout _descriptorSetLayout{};
		drhi::DynamicDescriptorPool _descriptorPool{};
		drhi::DynamicDescriptorSet _descriptorSet{};
		drhi::DynamicPipeline _pipeline{};
		drhi::DynamicPipelineLayout _pipelineLayout{};

	private:
		void* _uniformBufferMapped{ nullptr };
		drhi::DynamicBuffer               _uniformBuffer;
		drhi::DynamicDeviceMemory         _uniformBufferMemory;
		drhi::DynamicDescriptorBufferInfo _descriptorBufferInfo;

	public:
        void initialize(std::shared_ptr<drhi::DynamicRHI> rhi)
        {
            auto api = rhi->getCurrentAPI();
            auto bufferUsage = drhi::DynamicBufferUsageFlags(api);
            auto format = drhi::DynamicFormat(api);
            auto descriptorType = drhi::DynamicDescriptorType(api);
            auto stageFlags = drhi::DynamicShaderStageFlags(api);
            auto memoryFlags = drhi::DynamicMemoryPropertyFlagBits(api);
            auto cullMode = drhi::DynamicCullMode(api);
            auto sampleCount = drhi::DynamicSampleCountFlags(api);
            auto tilling = drhi::DynamicImageTiling(api);
            auto useFlag = drhi::DynamicImageUsageFlagBits(api);
            auto aspect = drhi::DynamicImageAspectFlagBits(api);
            auto memoryFlag = drhi::DynamicMemoryPropertyFlags(api);

            std::vector<drhi::DynamicDescriptorSetLayoutBinding> dsbs(3);
            dsbs[0].binding = 0;
            dsbs[0].descriptorCount = 1;
            dsbs[0].descriptorType = descriptorType.DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            dsbs[0].pImmutableSamplers = nullptr;
            dsbs[0].stageFlags = stageFlags.SHADER_STAGE_FRAGMENT_BIT;

            dsbs[1].binding = 1;
            dsbs[1].descriptorCount = 1;
            dsbs[1].descriptorType = descriptorType.DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            dsbs[1].pImmutableSamplers = nullptr;
            dsbs[1].stageFlags = stageFlags.SHADER_STAGE_FRAGMENT_BIT;

            dsbs[2].binding = 2;
            dsbs[2].descriptorCount = 1;
            dsbs[2].descriptorType = descriptorType.DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            dsbs[2].pImmutableSamplers = nullptr;
            dsbs[2].stageFlags = stageFlags.SHADER_STAGE_FRAGMENT_BIT;

            rhi->createDescriptorSetLayout(&_descriptorSetLayout, &dsbs);

            //create uniform buffer
            rhi->createUniformBuffer(&_uniformBuffer, &_uniformBufferMemory, &_uniformBufferMapped, sizeof(PostEffectPipelineUniformBuffer));
            _descriptorBufferInfo.set(rhi->getCurrentAPI(), _uniformBuffer, sizeof(PostEffectPipelineUniformBuffer));

            std::vector<drhi::DynamicDescriptorPoolSize> poolSizes(1);
            poolSizes[0].type = descriptorType.DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            poolSizes[0].descriptorCount = 3;

            // create descriptor
            rhi->createDescriptorPool(&_descriptorPool, &poolSizes);

            drhi::DynamicDescriptorImageInfo dii[2]{};
            dii[0].imageLayout = imageLayout.IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            dii[0].imageView = //uv tex;
            dii[0].sampler = ;

            dii[1].imageLayout = imageLayout.IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL;
            dii[1].imageView = // world tex;
            dii[1].sampler = ;

            std::vector<drhi::DynamicWriteDescriptorSet> wds(3);
            wds[0].descriptorType = descriptorType.DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            wds[0].dstBinding = 0;
            wds[0].pBufferInfo = &_descriptorBufferInfo;
            wds[0].descriptorCount = 1;

            wds[1].descriptorType = descriptorType.DESCRIPTOR_TYPE_SAMPLER;
            wds[1].dstBinding = 1;
            wds[1].pImageInfo = &dii[0];
            wds[1].descriptorCount = 1;

            wds[1].descriptorType = descriptorType.DESCRIPTOR_TYPE_SAMPLER;
            wds[1].dstBinding = 2;
            wds[1].pImageInfo = &dii[1];
            wds[1].descriptorCount = 1;

            rhi->createDescriptorSet(&_descriptorSet, &_descriptorSetLayout, &_descriptorPool, &wds, 0);

            // create pipeline
            drhi::DynamicPipelineCreateInfo pci = {};
            pci.fragmentShader = RESOURCE_PATH"Shaders/PostEffect/motionVectorsFragment.spv";
            pci.vertexInputBinding = drhi::DynamicVertexInputBindingDescription();
            pci.vertexInputBinding.set(api, 0, sizeof(Vertex));
            pci.vertexInputAttributes = std::vector<drhi::DynamicVertexInputAttributeDescription>();
            pci.vertexInputAttributes.resize(0);
            pci.colorImageFormat = format.FORMAT_B8G8R8A8_UNORM;
            pci.depthImageFormat = format.FORMAT_D32_SFLOAT_S8_UINT;
            pci.includeStencil = true;
            pci.dynamicDepthBias = false;
            pci.cullMode = cullMode.CULL_MODE_BACK_BIT;
            pci.sampleCounts = sampleCount.SAMPLE_COUNT_1_BIT;

            drhi::DynamicPipelineLayoutCreateInfo plci{};
            plci.pSetLayouts = &_descriptorSetLayout;
            plci.setLayoutCount = 1;
            plci.pushConstantRangeCount = 0;

            rhi->createPipelineLayout(&_pipelineLayout, &plci);

            rhi->createPipeline(&_pipeline, &_pipelineLayout, pci);
        }

        void updateUniformBuffer(UniformUpdateData uud)
        {
            PostEffectPipelineUniformBuffer ubo{};
            ubo.LastVP = uud.

            memcpy(_uniformBufferMapped, &ubo, sizeof(ubo));
        }

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

            rhi->cmdSetDepthBias(*commandBuffer, 0.5f, 0.0f, 2.0f);
            rhi->bindPipeline(_pipeline, commandBuffer, bindPoint.PIPELINE_BIND_POINT_GRAPHICS);
            rhi->bindDescriptorSets(&_descriptorSet, _pipelineLayout, commandBuffer, bindPoint.PIPELINE_BIND_POINT_GRAPHICS);
        }
	};
}