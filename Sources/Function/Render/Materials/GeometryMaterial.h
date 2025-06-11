#pragma once

#include<memory>

#include<drhi.h>

#include"Material.h"
#include"../Texture.h"
#include"../Geometry/MeshVertex.h"

namespace focus
{
    typedef struct GeometryUniformBufferObject
    {
        alignas(16) Matrix4 model;
        alignas(16) Matrix4 view;
        alignas(16) Matrix4 proj;
        alignas(16) Vector3 color;
    } GeometryUniformBufferObject;

    class GeometryMaterial : public Material
    {
    private:
        void* _vuniformBufferMapped{ nullptr };
        drhi::DynamicBuffer               _vuniformBuffer;
        drhi::DynamicDeviceMemory        _vuniformBufferMemory;
        drhi::DynamicDescriptorBufferInfo _vdescriptorBufferInfo;

    public:
        GeometryMaterial()
        {
            _type = "Geometry Material";
        }

        virtual void build(std::shared_ptr<drhi::DynamicRHI> rhi)
        {
            if (_built) return;
            auto api = rhi->getCurrentAPI();
            auto bufferUsage = drhi::DynamicBufferUsageFlags(api);
            auto format = drhi::DynamicFormat(api);
            auto descriptorType = drhi::DynamicDescriptorType(api);
            auto stageFlags = drhi::DynamicShaderStageFlags(api);
            auto memoryFlags = drhi::DynamicMemoryPropertyFlagBits(api);
            auto cullMode = drhi::DynamicCullMode(api);
            auto sampleCount = drhi::DynamicSampleCountFlags(api);

            if (_cullMode == 0)
                _cullMode = cullMode.CULL_MODE_BACK_BIT;

            std::vector<drhi::DynamicDescriptorSetLayoutBinding> dsbs(1);
            dsbs[0].binding = 0;
            dsbs[0].descriptorCount = 1;
            dsbs[0].descriptorType = descriptorType.DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            dsbs[0].pImmutableSamplers = nullptr;
            dsbs[0].stageFlags = stageFlags.SHADER_STAGE_VERTEX_BIT;

            rhi->createDescriptorSetLayout(&_descriptorSetLayout, &dsbs);

            //create uniform buffer
            rhi->createUniformBuffer(&_vuniformBuffer, &_vuniformBufferMemory, &_vuniformBufferMapped, sizeof(GeometryUniformBufferObject));
            _vdescriptorBufferInfo.set(rhi->getCurrentAPI(), _vuniformBuffer, sizeof(GeometryUniformBufferObject));

            std::vector<drhi::DynamicDescriptorPoolSize> poolSizes(1);
            poolSizes[0].type = descriptorType.DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            poolSizes[0].descriptorCount = 3;

            // create descriptor
            rhi->createDescriptorPool(&_descriptorPool, &poolSizes);

            std::vector<drhi::DynamicWriteDescriptorSet> wds(1);
            wds[0].descriptorType = descriptorType.DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            wds[0].dstBinding = 0;
            wds[0].pBufferInfo = &_vdescriptorBufferInfo;
            wds[0].descriptorCount = 1;

            rhi->createDescriptorSet(&_descriptorSet, &_descriptorSetLayout, &_descriptorPool, &wds, 0);

            // create pipeline
            drhi::DynamicPipelineCreateInfo pci = {};
            pci.vertexShader = RESOURCE_PATH"Shaders/Materials/geometryVertex.spv";
            pci.fragmentShader = RESOURCE_PATH"Shaders/Materials/geometryFragment.spv";
            pci.vertexInputBinding = drhi::DynamicVertexInputBindingDescription();
            pci.vertexInputBinding.set(api, 0, sizeof(Vertex));
            pci.vertexInputAttributes = std::vector<drhi::DynamicVertexInputAttributeDescription>();
            pci.vertexInputAttributes.resize(2);
            pci.vertexInputAttributes[0].set(api, 0, 0, format.FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Vertex::pos));
            pci.vertexInputAttributes[1].set(api, 1, 0, format.FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Vertex::color));
            pci.colorImageFormat = format.FORMAT_B8G8R8A8_UNORM;
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
        };

        virtual void updateUniformBuffer(UniformUpdateData uud)
        {
            GeometryUniformBufferObject ubo{};
            ubo.model = uud.model;
            ubo.view = uud.view;
            ubo.proj = uud.proj;
            ubo.color = uud.vertexColor;

            memcpy(_vuniformBufferMapped, &ubo, sizeof(ubo));
        }

        virtual void clean(std::shared_ptr<drhi::DynamicRHI> rhi)
        {
            rhi->clearBuffer(&_vuniformBuffer, &_vuniformBufferMemory);

            rhi->freeDescriptorSets(&_descriptorSet, &_descriptorPool);
            rhi->clearDescriptorPool(&_descriptorPool);
            rhi->clearDescriptorSetLayout(&_descriptorSetLayout);

            rhi->clearImage(&_textureImageView, &_textureImage, &_textureMemory);
            rhi->clearSampler(&_textureSampler);

            rhi->clearPipeline(&_pipeline, &_pipelineLayout);
        }
    };
}