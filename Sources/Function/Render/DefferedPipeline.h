#pragma once

#include<drhi.h>

#include"../../Core/Math.h"
#include"../../Core/Path.h"
#include"Geometry/MeshVertex.h"
#include"Pipeline.h"

namespace focus
{
    typedef struct DefferedUniformBufferObject
    {
        alignas(16) Matrix4 proj;
        alignas(16) Matrix4 model;
        alignas(16) Matrix4 view;
    } DefferedUniformBufferObject;

	class DefferedPipeline : public Pipeline
	{
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
            auto imageLayout = drhi::DynamicImageLayout(api);

            std::vector<drhi::DynamicDescriptorSetLayoutBinding> dsbs(2);
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

            rhi->createDescriptorSetLayout(&_descriptorSetLayout, &dsbs);

            //create uniform buffer
            rhi->createUniformBuffer(&_uniformBuffer, &_uniformBufferMemory, &_uniformBufferMapped, sizeof(DefferedUniformBufferObject));
            _descriptorBufferInfo.set(rhi->getCurrentAPI(), _uniformBuffer, sizeof(DefferedUniformBufferObject));

            std::vector<drhi::DynamicDescriptorPoolSize> poolSizes(2);
            poolSizes[0].type = descriptorType.DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            poolSizes[0].descriptorCount = 2;

            poolSizes[1].type = descriptorType.DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            poolSizes[1].descriptorCount = 2;

            // create descriptor
            rhi->createDescriptorPool(&_descriptorPool, &poolSizes);

            drhi::DynamicDescriptorImageInfo dii[1]{};
            dii[0].imageLayout = imageLayout.IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            dii[0].imageView = _textureImageView;
            dii[0].sampler = _textureSampler;

            std::vector<drhi::DynamicWriteDescriptorSet> wds(2);
            wds[0].descriptorType = descriptorType.DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            wds[0].dstBinding = 0;
            wds[0].pBufferInfo = &_descriptorBufferInfo;
            wds[0].descriptorCount = 1;

            wds[1].descriptorType = descriptorType.DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            wds[1].dstBinding = 1;
            wds[1].pImageInfo = &dii[0];
            wds[1].descriptorCount = 1;

            rhi->createDescriptorSet(&_descriptorSet, &_descriptorSetLayout, &_descriptorPool, &wds, 2);

            // create pipeline
            drhi::DynamicPipelineCreateInfo pci = {};
            pci.vertexShader = RESOURCE_PATH"Shaders/Deffered/gbufferVertex.spv";
            pci.fragmentShader = RESOURCE_PATH"Shaders/Deffered/gbufferFragment.spv";
            if (_isCube)
            {
                pci.vertexShader = RESOURCE_PATH"Shaders/Deffered/gbufferCubeVertex.spv";
                pci.fragmentShader = RESOURCE_PATH"Shaders/Deffered/gbufferCubeFragment.spv";
            }
            pci.vertexInputBinding = drhi::DynamicVertexInputBindingDescription();
            pci.vertexInputBinding.set(api, 0, sizeof(Vertex));
            pci.vertexInputAttributes = std::vector<drhi::DynamicVertexInputAttributeDescription>();
            pci.vertexInputAttributes.resize(3);
            pci.vertexInputAttributes[0].set(api, 0, 0, format.FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Vertex::pos));
            pci.vertexInputAttributes[1].set(api, 1, 0, format.FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Vertex::normal));
            pci.vertexInputAttributes[2].set(api, 2, 0, format.FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Vertex::texCoord));

            std::vector<uint32_t> colorFormats{};
            colorFormats.resize(3);
            colorFormats[0] = format.FORMAT_B8G8R8A8_UNORM;
            colorFormats[1] = format.FORMAT_R16G16B16A16_SFLOAT;
            colorFormats[2] = format.FORMAT_R16G16B16A16_SFLOAT;
            pci.colorImageFormats = colorFormats;
            pci.depthImageFormat = format.FORMAT_D32_SFLOAT_S8_UINT;
            pci.includeStencil = true;
            pci.dynamicDepthBias = false;
            pci.cullMode = cullMode.CULL_MODE_NONE;
            pci.sampleCounts = sampleCount.SAMPLE_COUNT_1_BIT;
            pci.colorAttachmentCount = 3;

            drhi::DynamicPipelineLayoutCreateInfo plci{};
            plci.pSetLayouts = &_descriptorSetLayout;
            plci.setLayoutCount = 1;
            plci.pushConstantRangeCount = 0;

            rhi->createPipelineLayout(&_pipelineLayout, &plci);

            rhi->createPipeline(&_pipeline, &_pipelineLayout, pci);
        }

        void updateUniformBuffer(UniformUpdateData uud)
        {
            DefferedUniformBufferObject ubo{};
            ubo.model = uud.model;
            ubo.view = uud.view;
            ubo.proj = uud.proj;

            memcpy(_uniformBufferMapped, &ubo, sizeof(ubo));
        }
	};
}