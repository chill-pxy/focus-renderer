#pragma once

#include"Material.h"
#include"../Texture.h"
#include"../Geometry/MeshVertex.h"

namespace focus
{
    typedef struct EUniformBufferObject
    {
        alignas(16) Matrix4 proj;
        alignas(16) Matrix4 model;
        alignas(16) Matrix4 view;
    } EUniformBufferObject;

    class EnvironmentMap : public Material
    {
    private:

        void* _uniformBufferMapped{ nullptr };
        drhi::DynamicBuffer               _uniformBuffer{};
        drhi::DynamicDeviceMemory         _uniformBufferMemory{};
        drhi::DynamicDescriptorBufferInfo _descriptorBufferInfo{};

        EUniformBufferObject      _uniformObject{};

    public:
        EnvironmentMap() = delete;
        EnvironmentMap(std::shared_ptr<Texture> texture)
        {
            _basicTexture = texture;
            _type = "EnvironmentMap Material";
        };

        virtual void build(std::shared_ptr<drhi::DynamicRHI> rhi)
        {
            if (_built) return;
            auto api = rhi->getCurrentAPI();
            auto bufferUsage = drhi::DynamicBufferUsageFlags(api);
            auto format = drhi::DynamicFormat(api);
            auto descriptorType = drhi::DynamicDescriptorType(api);
            auto imageLayout = drhi::DynamicImageLayout(api);
            auto imageAspect = drhi::DynamicImageAspectFlagBits(api);
            auto stageFlags = drhi::DynamicShaderStageFlags(api);
            auto memoryFlags = drhi::DynamicMemoryPropertyFlagBits(api);
            auto cullMode = drhi::DynamicCullMode(api);
            auto sampleCount = drhi::DynamicSampleCountFlags(api);

            if (_cullMode == 0)
                _cullMode = cullMode.CULL_MODE_BACK_BIT;

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
            rhi->createUniformBuffer(&_uniformBuffer, &_uniformBufferMemory, &_uniformBufferMapped, sizeof(EUniformBufferObject));
            _descriptorBufferInfo.set(rhi->getCurrentAPI(), _uniformBuffer, sizeof(EUniformBufferObject));

            std::vector<drhi::DynamicDescriptorPoolSize> poolSizes(2);
            poolSizes[0].type = descriptorType.DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            poolSizes[0].descriptorCount = 3;
            poolSizes[1].type = descriptorType.DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            poolSizes[1].descriptorCount = 3;

            // create descriptor
            rhi->createDescriptorPool(&_descriptorPool, &poolSizes);

            std::vector<drhi::DynamicWriteDescriptorSet> wds(2);
            wds[0].descriptorType = descriptorType.DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            wds[0].dstBinding = 0;
            wds[0].pBufferInfo = &_descriptorBufferInfo;
            wds[0].descriptorCount = 1;

            drhi::DynamicDescriptorImageInfo dii{};
            dii.imageLayout = imageLayout.IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            dii.imageView = _textureImageView;
            dii.sampler = _textureSampler;

            wds[1].descriptorType = descriptorType.DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            wds[1].dstBinding = 1;
            wds[1].descriptorCount = 1;
            wds[1].pImageInfo = &dii;

            rhi->createDescriptorSet(&_descriptorSet, &_descriptorSetLayout, &_descriptorPool, &wds, 1);

            // create pipeline
            drhi::DynamicPipelineCreateInfo pci = {};
            pci.vertexShader = RESOURCE_PATH"Shaders/Materials/environmentMapVertex.spv";
            pci.fragmentShader = RESOURCE_PATH"Shaders/Materials/environmentMapFragment.spv";
            pci.vertexInputBinding = drhi::DynamicVertexInputBindingDescription();
            pci.vertexInputBinding.set(api, 0, sizeof(Vertex));
            pci.vertexInputAttributes = std::vector<drhi::DynamicVertexInputAttributeDescription>();
            pci.vertexInputAttributes.resize(3);
            pci.vertexInputAttributes[0].set(api, 0, 0, format.FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Vertex::pos));
            pci.vertexInputAttributes[1].set(api, 1, 0, format.FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Vertex::normal));
            pci.vertexInputAttributes[2].set(api, 2, 0, format.FORMAT_R32G32_SFLOAT, offsetof(Vertex, Vertex::texCoord));
            pci.colorImageFormat = format.FORMAT_B8G8R8A8_SRGB;
            pci.depthImageFormat = format.FORMAT_D32_SFLOAT_S8_UINT;
            pci.includeStencil = true;
            pci.dynamicDepthBias = false;
            pci.cullMode = _cullMode;
            pci.sampleCounts = sampleCount.SAMPLE_COUNT_1_BIT;

            drhi::DynamicPipelineLayoutCreateInfo plci{};
            plci.pSetLayouts = &_descriptorSetLayout;
            plci.setLayoutCount = 1;
            plci.pushConstantRangeCount = 0;

            rhi->createPipelineLayout(&_pipelineLayout, &plci);

            rhi->createPipeline(&_pipeline, &_pipelineLayout, pci);

            _built = true;
        }

        virtual void updateUniformBuffer(UniformUpdateData uud)
        {
            EUniformBufferObject ubo{};
            ubo.model = uud.model;
            ubo.proj = uud.proj;
            ubo.view = uud.view;

            memcpy(_uniformBufferMapped, &ubo, sizeof(EUniformBufferObject));
        }

        virtual void clean(std::shared_ptr<drhi::DynamicRHI> rhi)
        {
            rhi->clearBuffer(&_uniformBuffer, &_uniformBufferMemory);

            rhi->freeDescriptorSets(&_descriptorSet, &_descriptorPool);
            rhi->clearDescriptorPool(&_descriptorPool);
            rhi->clearDescriptorSetLayout(&_descriptorSetLayout);

            rhi->clearImage(&_textureImageView, &_textureImage, &_textureMemory);
            rhi->clearSampler(&_textureSampler);

            rhi->clearPipeline(&_pipeline, &_pipelineLayout);
        }
    };
}