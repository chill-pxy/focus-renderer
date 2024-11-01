#pragma once

#include"Material.h"
#include"../Texture.h"
#include"../MeshVertex.h"

namespace FOCUS
{
	typedef struct UniformBufferObject
	{
		alignas(16) Matrix4 model;
		alignas(16) Matrix4 view;
		alignas(16) Matrix4 proj;
	} UniformBufferObject;

	class BasicMaterial : public Material
	{
	private:

        std::vector<void*>                             _uniformBuffersMapped;
        std::vector<DRHI::DynamicBuffer>               _uniformBuffers;
        std::vector<DRHI::DynamicDeviceMemory>         _uniformBuffersMemory;
        std::vector<DRHI::DynamicDescriptorBufferInfo> _descriptorBufferInfos;

		std::shared_ptr<Texture> _basicTexture{ nullptr };
        UniformBufferObject      _uniformObject{};

	public:
		BasicMaterial() = delete;
		BasicMaterial(std::shared_ptr<Texture> texture) : _basicTexture{ texture } {};

		virtual void build(std::shared_ptr<DRHI::DynamicRHI> rhi)
		{
            auto api = rhi->getCurrentAPI();
            auto bufferUsage = DRHI::DynamicBufferUsageFlags(api);
            auto format = DRHI::DynamicFormat(api);
            auto descriptorType = DRHI::DynamicDescriptorType(api);
            auto imageLayout = DRHI::DynamicImageLayout(api);
            auto stageFlags = DRHI::DynamicShaderStageFlags(api);
            auto memoryFlags = DRHI::DynamicMemoryPropertyFlagBits(api);

            // create descriptor
            rhi->createDescriptorPool(&_descriptorPool);

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

            rhi->createDescriptorSetLayout(&_descriptorSetLayout, &dsbs);

            //create uniform buffer
            rhi->createUniformBuffer(&_uniformBuffers, &_uniformBuffersMemory, &_uniformBuffersMapped, sizeof(UniformBufferObject));

            //binding sampler and image view
            rhi->createTextureImage(&_textureImage, &_textureMemory, _basicTexture->_width, _basicTexture->_height, _basicTexture->_channels, _basicTexture->_pixels);
            rhi->createImageView(&_textureImageView, &_textureImage, format.FORMAT_R8G8B8A8_SRGB);
            rhi->createTextureSampler(&_textureSampler);

            for (int i = 0; i < _uniformBuffers.size(); ++i)
            {
                DRHI::DynamicDescriptorBufferInfo bufferInfo;
                bufferInfo.set(rhi->getCurrentAPI(), _uniformBuffers[i], sizeof(UniformBufferObject));
                _descriptorBufferInfos.push_back(bufferInfo);
            }

            std::vector<DRHI::DynamicWriteDescriptorSet> wds(2);
            wds[0].descriptorType = descriptorType.DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            wds[0].dstBinding = 0;
            wds[0].pBufferInfo = &_descriptorBufferInfos[0];
            wds[0].descriptorCount = 1;

            DRHI::DynamicDescriptorImageInfo dii{};
            dii.imageLayout = imageLayout.IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            dii.imageView = _textureImageView;
            dii.sampler = _textureSampler;

            wds[1].descriptorType = descriptorType.DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            wds[1].dstBinding = 1;
            wds[1].pBufferInfo = &_descriptorBufferInfos[1];
            wds[1].descriptorCount = 1;
            wds[1].pImageInfo = &dii;

            rhi->createDescriptorSet(&_descriptorSet, &_descriptorSetLayout, &_descriptorPool, &wds);

            // create pipeline
            DRHI::DynamicPipelineCreateInfo pci = {};
            pci.vertexShader = "../../../Shaders/model_vertex.spv";
            pci.fragmentShader = "../../../Shaders/model_fragment.spv";
            pci.vertexInputBinding = DRHI::DynamicVertexInputBindingDescription();
            pci.vertexInputBinding.set(api, 0, sizeof(Vertex));
            pci.vertexInputAttributes = std::vector<DRHI::DynamicVertexInputAttributeDescription>();
            pci.vertexInputAttributes.resize(3);
            pci.vertexInputAttributes[0].set(api, 0, 0, format.FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Vertex::pos));
            pci.vertexInputAttributes[1].set(api, 1, 0, format.FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Vertex::color));
            pci.vertexInputAttributes[2].set(api, 2, 0, format.FORMAT_R32G32_SFLOAT, offsetof(Vertex, Vertex::texCoord));

            DRHI::DynamicPipelineLayoutCreateInfo plci{};
            plci.pSetLayouts = &_descriptorSetLayout;
            plci.setLayoutCount = 1;
            plci.pushConstantRangeCount = 0;

            rhi->createPipelineLayout(&_pipelineLayout, &plci);


            rhi->createPipeline(&_pipeline, &_pipelineLayout, pci);
		}

        virtual void updateUniformBuffer(uint32_t currentImage, std::shared_ptr<RenderCamera> camera)
        {
            UniformBufferObject ubo{};
            ubo.model = Matrix4(1.0f);
            ubo.view = camera->getViewMatrix();
            ubo.proj = perspective(radians(45.0f), 1280 / (float)720, 0.1f, 10.0f);
            ubo.proj[1][1] *= -1;

            memcpy(_uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
        }
	};
}