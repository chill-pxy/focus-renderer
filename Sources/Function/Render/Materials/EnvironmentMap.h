#pragma once

#include"Material.h"
#include"../Texture.h"
#include"../Geometry/MeshVertex.h"

namespace FOCUS
{
    typedef struct EUniformBufferObject
    {
        alignas(16) Matrix4 proj;
        alignas(16) Matrix4 model;
        alignas(16) Matrix4 view;
    } EUniformBufferObject;

    typedef struct FEUniformBufferObject
    {
        float exposure;
        float gamma;
    } FEUniformBufferObject;

    class EnvironmentMap : public Material
    {
    private:

        void* _vuniformBufferMapped{ nullptr };
        void* _funiformBufferMapped{ nullptr };
        DRHI::DynamicBuffer               _vuniformBuffer{};
        DRHI::DynamicBuffer               _funiformBuffer{};
        DRHI::DynamicDeviceMemory         _vuniformBufferMemory{};
        DRHI::DynamicDeviceMemory         _funiformBufferMemory{};
        DRHI::DynamicDescriptorBufferInfo _vdescriptorBufferInfo{};
        DRHI::DynamicDescriptorBufferInfo _fdescriptorBufferInfo{};

        std::shared_ptr<Texture> _basicTexture{ nullptr };
        EUniformBufferObject      _uniformObject{};

        bool _isCube{ false };

    public:
        EnvironmentMap() = delete;
        EnvironmentMap(std::shared_ptr<Texture> texture, bool isCube = false) : _basicTexture{ texture }, _isCube{isCube} {};

        virtual void build(std::shared_ptr<DRHI::DynamicRHI> rhi, DRHI::DynamicCommandPool* commandPool, DRHI::DynamicImage shadowImage, DRHI::DynamicImageView shadowImageView, DRHI::DynamicSampler shadowSampler)
        {
            if (_built) return;
            auto api = rhi->getCurrentAPI();
            auto bufferUsage = DRHI::DynamicBufferUsageFlags(api);
            auto format = DRHI::DynamicFormat(api);
            auto descriptorType = DRHI::DynamicDescriptorType(api);
            auto imageLayout = DRHI::DynamicImageLayout(api);
            auto imageAspect = DRHI::DynamicImageAspectFlagBits(api);
            auto stageFlags = DRHI::DynamicShaderStageFlags(api);
            auto memoryFlags = DRHI::DynamicMemoryPropertyFlagBits(api);
            auto cullMode = DRHI::DynamicCullMode(api);
            auto sampleCount = DRHI::DynamicSampleCountFlags(api);

            if (_cullMode == 0)
                _cullMode = cullMode.CULL_MODE_BACK_BIT;

            std::vector<DRHI::DynamicDescriptorSetLayoutBinding> dsbs(3);
            dsbs[0].binding = 0;
            dsbs[0].descriptorCount = 1;
            dsbs[0].descriptorType = descriptorType.DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            dsbs[0].pImmutableSamplers = nullptr;
            dsbs[0].stageFlags = stageFlags.SHADER_STAGE_VERTEX_BIT;

            dsbs[1].binding = 1;
            dsbs[1].descriptorCount = 1;
            dsbs[1].descriptorType = descriptorType.DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            dsbs[1].pImmutableSamplers = nullptr;
            dsbs[1].stageFlags = stageFlags.SHADER_STAGE_FRAGMENT_BIT;

            dsbs[2].binding = 2;
            dsbs[2].descriptorCount = 1;
            dsbs[2].descriptorType = descriptorType.DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            dsbs[2].pImmutableSamplers = nullptr;
            dsbs[2].stageFlags = stageFlags.SHADER_STAGE_FRAGMENT_BIT;

            rhi->createDescriptorSetLayout(&_descriptorSetLayout, &dsbs);

            //create uniform buffer
            rhi->createUniformBuffer(&_vuniformBuffer, &_vuniformBufferMemory, &_vuniformBufferMapped, sizeof(EUniformBufferObject));
            _vdescriptorBufferInfo.set(rhi->getCurrentAPI(), _vuniformBuffer, sizeof(EUniformBufferObject));
            rhi->createUniformBuffer(&_funiformBuffer, &_funiformBufferMemory, &_funiformBufferMapped, sizeof(FEUniformBufferObject));
            _fdescriptorBufferInfo.set(rhi->getCurrentAPI(), _funiformBuffer, sizeof(FEUniformBufferObject));

            //binding sampler and image view
            auto imageFlag = DRHI::DynamicImageCreateFlags(api);
            DRHI::DynamicImageCreateInfo ici{};
            ici.arrayLayers = 6;
            ici.extent.width = _basicTexture->_width;
            ici.extent.height = _basicTexture->_height;
            ici.extent.depth = 1;
            ici.flags = imageFlag.IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
            ici.mipLevels = 6;
            rhi->createTextureImage(&_textureImage, &_textureMemory, ici, commandPool, _basicTexture->_pixels);
            
            if (_isCube)
            {
                auto type = DRHI::DynamicImageViewType(api);
                DRHI::DynamicImageViewCreateInfo viewInfo{};
                viewInfo.format = format.FORMAT_B8G8R8A8_SRGB;
                viewInfo.image = _textureImage;
                viewInfo.type = type.IMAGE_VIEW_TYPE_CUBE;
                viewInfo.subresourceRange.aspectMask = imageAspect.IMAGE_ASPECT_COLOR_BIT;
                viewInfo.subresourceRange.layerCount = 6;
                viewInfo.subresourceRange.baseArrayLayer = 0;
                viewInfo.subresourceRange.baseMipLevel = 0;
                viewInfo.subresourceRange.levelCount = 1;
                rhi->createImageView(&_textureImageView, &_textureImage, viewInfo);
            }
            else
            {
                rhi->createImageView(&_textureImageView, &_textureImage, format.FORMAT_R8G8B8A8_SRGB, imageAspect.IMAGE_ASPECT_COLOR_BIT);
            }
           
            rhi->createTextureSampler(&_textureSampler);

            std::vector<DRHI::DynamicDescriptorPoolSize> poolSizes(3);
            poolSizes[0].type = descriptorType.DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            poolSizes[0].descriptorCount = 3;
            poolSizes[1].type = descriptorType.DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            poolSizes[1].descriptorCount = 3;
            poolSizes[2].type = descriptorType.DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            poolSizes[2].descriptorCount = 3;

            // create descriptor
            rhi->createDescriptorPool(&_descriptorPool, &poolSizes);

            std::vector<DRHI::DynamicWriteDescriptorSet> wds(3);
            wds[0].descriptorType = descriptorType.DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            wds[0].dstBinding = 0;
            wds[0].pBufferInfo = &_vdescriptorBufferInfo;
            wds[0].descriptorCount = 1;

            wds[1].descriptorType = descriptorType.DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            wds[1].dstBinding = 1;
            wds[1].pBufferInfo = &_fdescriptorBufferInfo;
            wds[1].descriptorCount = 1;

            DRHI::DynamicDescriptorImageInfo dii{};
            dii.imageLayout = imageLayout.IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            dii.imageView = _textureImageView;
            dii.sampler = _textureSampler;

            wds[2].descriptorType = descriptorType.DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            wds[2].dstBinding = 2;
            wds[2].descriptorCount = 1;
            wds[2].pImageInfo = &dii;

            rhi->createDescriptorSet(&_descriptorSet, &_descriptorSetLayout, &_descriptorPool, &wds, 1);

            // create pipeline
            DRHI::DynamicPipelineCreateInfo pci = {};
            if (_isCube)
            {
                pci.vertexShader = "../../../Shaders/Materials/environmentCubeVertex.spv";
                pci.fragmentShader = "../../../Shaders/Materials/environmentCubeFragment.spv";
            }
            else
            {
                pci.vertexShader = "../../../Shaders/Materials/environmentMapVertex.spv";
                pci.fragmentShader = "../../../Shaders/Materials/environmentMapFragment.spv";
            }
            pci.vertexInputBinding = DRHI::DynamicVertexInputBindingDescription();
            pci.vertexInputBinding.set(api, 0, sizeof(Vertex));
            pci.vertexInputAttributes = std::vector<DRHI::DynamicVertexInputAttributeDescription>();
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

            DRHI::DynamicPipelineLayoutCreateInfo plci{};
            plci.pSetLayouts = &_descriptorSetLayout;
            plci.setLayoutCount = 1;
            plci.pushConstantRangeCount = 0;

            rhi->createPipelineLayout(&_pipelineLayout, &plci);

            rhi->createPipeline(&_pipeline, &_pipelineLayout, pci);

            _built = true;
        }

        virtual void updateUniformBuffer(UniformUpdateData uud)
        {
           // if (_isCube)
           // {
                FEUniformBufferObject fe{};
                fe.exposure = 1.0;
                fe.gamma = 1.0;
                memcpy(_funiformBufferMapped, &fe, sizeof(FEUniformBufferObject));
           // }

            EUniformBufferObject ubo{};
            ubo.model = uud.model;
            ubo.proj = uud.proj;
            ubo.view = uud.view;

            memcpy(_vuniformBufferMapped, &ubo, sizeof(EUniformBufferObject));

        }

        virtual void clean(std::shared_ptr<DRHI::DynamicRHI> rhi)
        {
            rhi->clearBuffer(&_vuniformBuffer, &_vuniformBufferMemory);
            rhi->clearBuffer(&_funiformBuffer, &_funiformBufferMemory);

            rhi->freeDescriptorSets(&_descriptorSet, &_descriptorPool);
            rhi->clearDescriptorPool(&_descriptorPool);
            rhi->clearDescriptorSetLayout(&_descriptorSetLayout);

            rhi->clearImage(&_textureImageView, &_textureImage, &_textureMemory);
            rhi->clearSampler(&_textureSampler);

            rhi->clearPipeline(&_pipeline, &_pipelineLayout);
        }
    };
}