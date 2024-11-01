#pragma once

#include<memory>

#include<drhi.h>

#include"Material.h"
#include"../Texture.h"
#include"../MeshVertex.h"

namespace FOCUS
{
	typedef struct VertexUniformBufferObject
	{
		alignas(16) Matrix4 model;
		alignas(16) Matrix4 view;
		alignas(16) Matrix4 proj;
	} VertexUniformBufferObject;

    typedef struct FragmentUniformBufferObject
    {
        alignas(16) Vector3 lightPosition;
        alignas(16) Vector3 viewPosition;
    } FragmentUniformBufferObject;

	class BlinnPhongMaterial : public Material
	{
	private:
        std::vector<void*>                             _vuniformBuffersMapped;
        std::vector<DRHI::DynamicBuffer>               _vuniformBuffers;
        std::vector<DRHI::DynamicDeviceMemory>         _vuniformBuffersMemory;
        std::vector<DRHI::DynamicDescriptorBufferInfo> _vdescriptorBufferInfos;

        std::vector<void*>                             _funiformBuffersMapped;
        std::vector<DRHI::DynamicBuffer>               _funiformBuffers;
        std::vector<DRHI::DynamicDeviceMemory>         _funiformBuffersMemory;
        std::vector<DRHI::DynamicDescriptorBufferInfo> _fdescriptorBufferInfos;
        
        std::shared_ptr<Texture> _basicTexture;

	public:
		BlinnPhongMaterial() = delete;
        BlinnPhongMaterial(std::shared_ptr<Texture> texture) :_basicTexture{ texture } {}
		
		virtual void build(std::shared_ptr<DRHI::DynamicRHI> rhi) 
		{
            auto api = rhi->getCurrentAPI();
            auto bufferUsage = DRHI::DynamicBufferUsageFlags(api);
            auto format = DRHI::DynamicFormat(api);
            auto descriptorType = DRHI::DynamicDescriptorType(api);
            auto imageLayout = DRHI::DynamicImageLayout(api);
            auto stageFlags = DRHI::DynamicShaderStageFlags(api);
            auto memoryFlags = DRHI::DynamicMemoryPropertyFlagBits(api);

            std::vector<DRHI::DynamicDescriptorSetLayoutBinding> dsbs(3);
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
            dsbs[2].descriptorType = descriptorType.DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            dsbs[2].pImmutableSamplers = nullptr;
            dsbs[2].stageFlags = stageFlags.SHADER_STAGE_FRAGMENT_BIT;

            rhi->createDescriptorSetLayout(&_descriptorSetLayout, &dsbs);

            //create uniform buffer
            rhi->createUniformBuffer(&_vuniformBuffers, &_vuniformBuffersMemory, &_vuniformBuffersMapped, sizeof(VertexUniformBufferObject));
            for (int i = 0; i < _vuniformBuffers.size(); ++i)
            {
                DRHI::DynamicDescriptorBufferInfo vbufferInfo;
                vbufferInfo.set(rhi->getCurrentAPI(), _vuniformBuffers[i], sizeof(VertexUniformBufferObject));
                _vdescriptorBufferInfos.push_back(vbufferInfo);
            }
            
            rhi->createUniformBuffer(&_funiformBuffers, &_funiformBuffersMemory, &_funiformBuffersMapped, sizeof(FragmentUniformBufferObject));
            for (int i = 0; i < _funiformBuffers.size(); ++i)
            {
                DRHI::DynamicDescriptorBufferInfo fbufferInfo;
                fbufferInfo.set(rhi->getCurrentAPI(), _funiformBuffers[i], sizeof(FragmentUniformBufferObject));
                _fdescriptorBufferInfos.push_back(fbufferInfo);
            }

            //binding sampler and image view
            rhi->createTextureImage(&_textureImage, &_textureMemory, _basicTexture->_width, _basicTexture->_height, _basicTexture->_channels, _basicTexture->_pixels);
            rhi->createImageView(&_textureImageView, &_textureImage, format.FORMAT_R8G8B8A8_SRGB);
            rhi->createTextureSampler(&_textureSampler);

            std::vector<DRHI::DynamicDescriptorPoolSize> poolSizes(3);
            poolSizes[0].type = descriptorType.DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            poolSizes[0].descriptorCount = 3;
            poolSizes[1].type = descriptorType.DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            poolSizes[1].descriptorCount = 3;
            poolSizes[2].type = descriptorType.DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            poolSizes[2].descriptorCount = 3;

            // create descriptor
            rhi->createDescriptorPool(&_descriptorPool, &poolSizes);

            std::vector<DRHI::DynamicWriteDescriptorSet> wds(3);
            wds[0].descriptorType = descriptorType.DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            wds[0].dstBinding = 0;
            wds[0].pBufferInfo = &_vdescriptorBufferInfos[0];
            wds[0].descriptorCount = 1;

            DRHI::DynamicDescriptorImageInfo dii{};
            dii.imageLayout = imageLayout.IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            dii.imageView = _textureImageView;
            dii.sampler = _textureSampler;

            wds[1].descriptorType = descriptorType.DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            wds[1].dstBinding = 1;
            wds[1].descriptorCount = 1;
            wds[1].pImageInfo = &dii;

            wds[2].descriptorType = descriptorType.DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            wds[2].dstBinding = 2;
            wds[2].pBufferInfo = &_fdescriptorBufferInfos[0];
            wds[2].descriptorCount = 1;

            rhi->createDescriptorSet(&_descriptorSet, &_descriptorSetLayout, &_descriptorPool, &wds);

            // create pipeline
            DRHI::DynamicPipelineCreateInfo pci = {};
            pci.vertexShader = "../../../Shaders/blinnPhongVertex.spv";
            pci.fragmentShader = "../../../Shaders/blinnPhongFragment.spv";
            pci.vertexInputBinding = DRHI::DynamicVertexInputBindingDescription();
            pci.vertexInputBinding.set(api, 0, sizeof(Vertex));
            pci.vertexInputAttributes = std::vector<DRHI::DynamicVertexInputAttributeDescription>();
            pci.vertexInputAttributes.resize(4);
            pci.vertexInputAttributes[0].set(api, 0, 0, format.FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Vertex::pos));
            pci.vertexInputAttributes[1].set(api, 1, 0, format.FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Vertex::color));
            pci.vertexInputAttributes[2].set(api, 2, 0, format.FORMAT_R32G32_SFLOAT, offsetof(Vertex, Vertex::texCoord));
            pci.vertexInputAttributes[3].set(api, 3, 0, format.FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Vertex::normal));

            DRHI::DynamicPipelineLayoutCreateInfo plci{};
            plci.pSetLayouts = &_descriptorSetLayout;
            plci.setLayoutCount = 1;
            plci.pushConstantRangeCount = 0;

            rhi->createPipelineLayout(&_pipelineLayout, &plci);


            rhi->createPipeline(&_pipeline, &_pipelineLayout, pci);
		};

		virtual void updateUniformBuffer(uint32_t currentImage, std::shared_ptr<RenderCamera> camera)
        {
            VertexUniformBufferObject vubo{};
            vubo.model = Matrix4(1.0f);
            vubo.view = camera->getViewMatrix();
            vubo.proj = perspective(radians(45.0f), 1280 / (float)720, 0.1f, 10.0f);
            vubo.proj[1][1] *= -1;
            memcpy(_vuniformBuffersMapped[currentImage], &vubo, sizeof(vubo));

            FragmentUniformBufferObject fubo{};
            fubo.lightPosition = Vector3(0.0f, 0.0f, 0.0f);
            fubo.viewPosition = camera->_position;
            memcpy(_funiformBuffersMapped[currentImage], &fubo, sizeof(fubo));
        }
	};
}