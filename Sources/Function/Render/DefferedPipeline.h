#pragma once

#include<drhi.h>

#include"../../Core/Math.h"
#include"../../Core/Path.h"
#include"Geometry/MeshVertex.h"

namespace focus
{
    typedef struct DefferedUniformBufferObject
    {
        alignas(16) Matrix4 proj;
        alignas(16) Matrix4 model;
        alignas(16) Matrix4 view;
    } DefferedUniformBufferObject;

	class DefferedPipeline
	{
    public:
        drhi::DynamicDescriptorSetLayout _descriptorSetLayout{};
        drhi::DynamicDescriptorPool _descriptorPool{};
        drhi::DynamicDescriptorSet _descriptorSet{};
        drhi::DynamicPipeline _pipeline{};
        drhi::DynamicPipelineLayout _pipelineLayout{};

        // G-buffer
        drhi::DynamicImage _position{};
        drhi::DynamicImageView _positionView{};
        drhi::DynamicSampler _positionSampler{};

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

            std::vector<drhi::DynamicDescriptorSetLayoutBinding> dsbs(1);
            dsbs[0].binding = 0;
            dsbs[0].descriptorCount = 1;
            dsbs[0].descriptorType = descriptorType.DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            dsbs[0].pImmutableSamplers = nullptr;
            dsbs[0].stageFlags = stageFlags.SHADER_STAGE_VERTEX_BIT;

            rhi->createDescriptorSetLayout(&_descriptorSetLayout, &dsbs);

            //create uniform buffer
            rhi->createUniformBuffer(&_uniformBuffer, &_uniformBufferMemory, &_uniformBufferMapped, sizeof(DefferedUniformBufferObject));
            _descriptorBufferInfo.set(rhi->getCurrentAPI(), _uniformBuffer, sizeof(DefferedUniformBufferObject));

            std::vector<drhi::DynamicDescriptorPoolSize> poolSizes(1);
            poolSizes[0].type = descriptorType.DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            poolSizes[0].descriptorCount = 3;

            // create descriptor
            rhi->createDescriptorPool(&_descriptorPool, &poolSizes);

            std::vector<drhi::DynamicWriteDescriptorSet> wds(1);
            wds[0].descriptorType = descriptorType.DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            wds[0].dstBinding = 0;
            wds[0].pBufferInfo = &_descriptorBufferInfo;
            wds[0].descriptorCount = 1;

            rhi->createDescriptorSet(&_descriptorSet, &_descriptorSetLayout, &_descriptorPool, &wds, 0);

            // create pipeline
            drhi::DynamicPipelineCreateInfo pci = {};
            pci.vertexShader = RESOURCE_PATH"Shaders/Deffered/normalVertex.spv";
            pci.fragmentShader = RESOURCE_PATH"Shaders/Deffered/normalFragment.spv";
            pci.vertexInputBinding = drhi::DynamicVertexInputBindingDescription();
            pci.vertexInputBinding.set(api, 0, sizeof(Vertex));
            pci.vertexInputAttributes = std::vector<drhi::DynamicVertexInputAttributeDescription>();
            pci.vertexInputAttributes.resize(3);
            pci.vertexInputAttributes[0].set(api, 0, 0, format.FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Vertex::pos));
            pci.vertexInputAttributes[1].set(api, 1, 0, format.FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Vertex::normal));
            pci.vertexInputAttributes[2].set(api, 2, 0, format.FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Vertex::texCoord));
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

        virtual void updateUniformBuffer(UniformUpdateData uud)
        {
            DefferedUniformBufferObject ubo{};
            ubo.model = uud.model;
            ubo.view = uud.view;
            ubo.proj = uud.proj;

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