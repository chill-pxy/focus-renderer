#pragma once

#include<memory>

#include<drhi.h>

#include"Material.h"
#include"../Texture.h"
#include"../Geometry/MeshVertex.h"

namespace focus
{
    typedef struct PhysicalUniformBufferObject
    {
        alignas(16) Matrix4 model;
        alignas(16) Matrix4 view;
        alignas(16) Matrix4 proj;

        alignas(16) Vector3 viewPosition;

        alignas(16) Vector3 pointLightPosition;
        alignas(16) Vector3 pointLightColor;
        alignas(4) float    pointLightIntensity;

        alignas(16) Matrix4 dirLightSpace;
        alignas(16) Vector3 dirLightDirection;
        alignas(16) Vector3 dirLightColor;
        alignas(4) float    dirLightStrength;

        alignas(4) float metallic;
        alignas(4) float roughness;

    } PhysicalUniformBufferObject;

    class PhysicalMaterial : public Material
    {
    public:
        std::shared_ptr<Texture> _basicTexture;

    private:
        void* _vuniformBufferMapped{ nullptr };
        drhi::DynamicBuffer               _vuniformBuffer;
        drhi::DynamicDeviceMemory        _vuniformBufferMemory;
        drhi::DynamicDescriptorBufferInfo _vdescriptorBufferInfo;

    public:
        PhysicalMaterial() {};
        PhysicalMaterial(std::shared_ptr<Texture> texture) :_basicTexture{ texture } {
            _type = "Physical Material";
        }

        virtual void build(std::shared_ptr<drhi::DynamicRHI> rhi, drhi::DynamicCommandPool* commandPool)
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
                _cullMode = cullMode.CULL_MODE_NONE;

            std::vector<drhi::DynamicDescriptorSetLayoutBinding> dsbs(7);
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

            dsbs[2].binding = 2;
            dsbs[2].descriptorCount = 1;
            dsbs[2].descriptorType = descriptorType.DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            dsbs[2].pImmutableSamplers = nullptr;
            dsbs[2].stageFlags = stageFlags.SHADER_STAGE_FRAGMENT_BIT;

            dsbs[3].binding = 3;
            dsbs[3].descriptorCount = 1;
            dsbs[3].descriptorType = descriptorType.DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            dsbs[3].pImmutableSamplers = nullptr;
            dsbs[3].stageFlags = stageFlags.SHADER_STAGE_FRAGMENT_BIT;

            dsbs[4].binding = 4;
            dsbs[4].descriptorCount = 1;
            dsbs[4].descriptorType = descriptorType.DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            dsbs[4].pImmutableSamplers = nullptr;
            dsbs[4].stageFlags = stageFlags.SHADER_STAGE_FRAGMENT_BIT;

            dsbs[5].binding = 5;
            dsbs[5].descriptorCount = 1;
            dsbs[5].descriptorType = descriptorType.DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            dsbs[5].pImmutableSamplers = nullptr;
            dsbs[5].stageFlags = stageFlags.SHADER_STAGE_FRAGMENT_BIT;

            dsbs[6].binding = 6;
            dsbs[6].descriptorCount = 1;
            dsbs[6].descriptorType = descriptorType.DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            dsbs[6].pImmutableSamplers = nullptr;
            dsbs[6].stageFlags = stageFlags.SHADER_STAGE_FRAGMENT_BIT;

            rhi->createDescriptorSetLayout(&_descriptorSetLayout, &dsbs);

            //create uniform buffer
            rhi->createUniformBuffer(&_vuniformBuffer, &_vuniformBufferMemory, &_vuniformBufferMapped, sizeof(PhysicalUniformBufferObject));
            _vdescriptorBufferInfo.set(rhi->getCurrentAPI(), _vuniformBuffer, sizeof(PhysicalUniformBufferObject));

            //binding sampler and image view
            rhi->createTextureImage(&_textureImage, &_textureMemory, commandPool, _basicTexture->_width, _basicTexture->_height, _basicTexture->_channels, _basicTexture->_pixels);
            rhi->createImageView(&_textureImageView, &_textureImage, format.FORMAT_R8G8B8A8_SRGB, imageAspect.IMAGE_ASPECT_COLOR_BIT);
            rhi->createTextureSampler(&_textureSampler);

            std::vector<drhi::DynamicDescriptorPoolSize> poolSizes(7);
            poolSizes[0].type = descriptorType.DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            poolSizes[0].descriptorCount = 7;
            poolSizes[1].type = descriptorType.DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            poolSizes[1].descriptorCount = 7;
            poolSizes[2].type = descriptorType.DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            poolSizes[2].descriptorCount = 7;
            poolSizes[3].type = descriptorType.DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            poolSizes[3].descriptorCount = 7;
            poolSizes[4].type = descriptorType.DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            poolSizes[4].descriptorCount = 7;
            poolSizes[5].type = descriptorType.DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            poolSizes[5].descriptorCount = 7;
            poolSizes[6].type = descriptorType.DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            poolSizes[6].descriptorCount = 7;

            // create descriptor
            rhi->createDescriptorPool(&_descriptorPool, &poolSizes);

            drhi::DynamicDescriptorImageInfo dii[6]{};
            dii[0].imageLayout = imageLayout.IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            dii[0].imageView = _textureImageView;
            dii[0].sampler = _textureSampler;

            dii[1].imageLayout = imageLayout.IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL;
            dii[1].imageView = *_shadowImageView;
            dii[1].sampler = *_shadowSampler;

            dii[2].imageLayout = imageLayout.IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            dii[2].imageView = *_brdfImageView;
            dii[2].sampler = *_brdfSampler;

            dii[3].imageLayout = imageLayout.IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            dii[3].imageView = *_irradianceImageView;
            dii[3].sampler = *_irradianceSampler;

            dii[4].imageLayout = imageLayout.IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            dii[4].imageView = *_filteredImageView;
            dii[4].sampler = *_filteredImageSampler;

            dii[5].imageLayout = imageLayout.IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            dii[5].imageView = *_gbuffer.normalImageView;
            dii[5].sampler = *_gbuffer.normalSampler;

            std::vector<drhi::DynamicWriteDescriptorSet> wds(7);
            wds[0].descriptorType = descriptorType.DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            wds[0].dstBinding = 0;
            wds[0].pBufferInfo = &_vdescriptorBufferInfo;
            wds[0].descriptorCount = 1;

            wds[1].descriptorType = descriptorType.DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            wds[1].dstBinding = 1;
            wds[1].descriptorCount = 1;
            wds[1].pImageInfo = &dii[0];

            wds[2].descriptorType = descriptorType.DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            wds[2].dstBinding = 2;
            wds[2].descriptorCount = 1;
            wds[2].pImageInfo = &dii[1];

            wds[3].descriptorType = descriptorType.DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            wds[3].dstBinding = 3;
            wds[3].descriptorCount = 1;
            wds[3].pImageInfo = &dii[2];

            wds[4].descriptorType = descriptorType.DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            wds[4].dstBinding = 4;
            wds[4].descriptorCount = 1;
            wds[4].pImageInfo = &dii[3];

            wds[5].descriptorType = descriptorType.DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            wds[5].dstBinding = 5;
            wds[5].descriptorCount = 1;
            wds[5].pImageInfo = &dii[4];

            wds[6].descriptorType = descriptorType.DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            wds[6].dstBinding = 6;
            wds[6].descriptorCount = 1;
            wds[6].pImageInfo = &dii[5];

            rhi->createDescriptorSet(&_descriptorSet, &_descriptorSetLayout, &_descriptorPool, &wds, 7);

            // create pipeline
            drhi::DynamicPipelineCreateInfo pci = {};
            pci.vertexShader = RESOURCE_PATH"Shaders/Materials/physicalMaterialVertex.spv";
            pci.fragmentShader = RESOURCE_PATH"Shaders/Materials/physicalMaterialFragment.spv";
            pci.vertexInputBinding = drhi::DynamicVertexInputBindingDescription();
            pci.vertexInputBinding.set(api, 0, sizeof(Vertex));
            pci.vertexInputAttributes = std::vector<drhi::DynamicVertexInputAttributeDescription>();
            pci.vertexInputAttributes.resize(4);
            pci.vertexInputAttributes[0].set(api, 0, 0, format.FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Vertex::pos));
            pci.vertexInputAttributes[1].set(api, 1, 0, format.FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Vertex::color));
            pci.vertexInputAttributes[2].set(api, 2, 0, format.FORMAT_R32G32_SFLOAT, offsetof(Vertex, Vertex::texCoord));
            pci.vertexInputAttributes[3].set(api, 3, 0, format.FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Vertex::normal));
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
            PhysicalUniformBufferObject ubo{};
            ubo.model = uud.model;
            ubo.view = uud.view;
            ubo.proj = uud.proj;

            ubo.viewPosition = uud.viewPosition;

            ubo.pointLightPosition = uud.pointLightPosition;
            ubo.pointLightColor = uud.pointLightColor;
            ubo.pointLightIntensity = uud.pointLightIntensity;

            ubo.dirLightSpace = uud.dirLightSpace;
            ubo.dirLightDirection = uud.dirLightDirection;
            ubo.dirLightColor = uud.dirLightColor;
            ubo.dirLightStrength = uud.dirLightStrength;

            ubo.metallic = _metallic;
            ubo.roughness = _roughness;

            memcpy(_vuniformBufferMapped, &ubo, sizeof(ubo));
        }

        virtual void clean(std::shared_ptr<drhi::DynamicRHI> rhi)
        {
            if (_cleared) return;
            rhi->clearBuffer(&_vuniformBuffer, &_vuniformBufferMemory);

            rhi->freeDescriptorSets(&_descriptorSet, &_descriptorPool);
            rhi->clearDescriptorPool(&_descriptorPool);
            rhi->clearDescriptorSetLayout(&_descriptorSetLayout);

            rhi->clearImage(&_textureImageView, &_textureImage, &_textureMemory);
            rhi->clearSampler(&_textureSampler);

            rhi->clearPipeline(&_pipeline, &_pipelineLayout);

            _cleared = true;
        }
    };
}