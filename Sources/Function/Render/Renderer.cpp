#include<chrono>

#include"Renderer.h"
#include"RenderResource.h"

namespace FOCUS
{
	Renderer::Renderer(DRHI::API api, DRHI::PlatformInfo platformCI)
	{
		switch (api)
		{
		default:
		case DRHI::VULKAN:
			DRHI::RHICreateInfo rhiCI{};
			rhiCI.platformInfo = platformCI;
			_rhiContext = std::make_shared<DRHI::VulkanDRHI>(rhiCI);

			_ui = std::make_shared<EngineUI>(platformCI.window);
			break;
		}
	}

	void Renderer::initialize()
	{
		//-------------------------------initialize variant------------------------------------
		auto api = _rhiContext->getCurrentAPI();
		auto format = DRHI::DynamicFormat(api);
		
		_rhiContext->initialize();
		//-------------------------------------------------------------------------------------



		//--------------------------------build render resources-------------------------------
		obj = std::shared_ptr<Mesh>(loadModel("../../../Asset/Models/viking_room.obj"));
		std::shared_ptr<Texture> texture = std::shared_ptr<Texture>(loadTexture("../../../Asset/Models/viking_room.png"));
		obj->_texture = texture;
		obj->build(_rhiContext);

		_ui->initialize();
		_ui->build(_rhiContext);
		_ui->preparePipeline(_rhiContext);
		//-------------------------------------------------------------------------------------



		//--------------------------------create pipeline--------------------------------------
		DRHI::PipelineCreateInfo pci = {};
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
		plci.pSetLayouts = &obj->_descriptorSetLayout;
		plci.setLayoutCount = 1;
		plci.pushConstantRangeCount = 0;

		_rhiContext->createPipelineLayout(&modelPipelineLayout, &plci);


		_rhiContext->createPipeline(&modelPipeline, &modelPipelineLayout, pci);
		//------------------------------------------------------------------------------------
		
		buildCommandBuffer();
	}

	void Renderer::buildCommandBuffer()
	{
		auto api = _rhiContext->getCurrentAPI();
		auto bindPoint = DRHI::DynamicPipelineBindPoint(api);

		for (uint32_t i = 0; i < _rhiContext->getCommandBufferSize(); ++i)
		{
			_rhiContext->beginCommandBuffer(i);

			_rhiContext->bindPipeline(modelPipeline, bindPoint.PIPELINE_BIND_POINT_GRAPHICS, i);
			_rhiContext->bindVertexBuffers(&obj->_vertexBuffer, i);
			_rhiContext->bindIndexBuffer(&obj->_indexBuffer, i);
			_rhiContext->bindDescriptorSets(&obj->_descriptorSet, modelPipelineLayout, 0, i);

			//draw model
			_rhiContext->drawIndexed(i, static_cast<uint32_t>(obj->_indices.size()), 1, 0, 0, 0);

			_ui->draw(i, _rhiContext);

			_rhiContext->endCommandBuffer(i);
		}
	}

	void Renderer::tick()
	{
		//_rhiContext->frameOnTick(buildCommandBuffer);
		updateUniformBuffer(_rhiContext->getCurrentBuffer());
	}

	void Renderer::clean()
	{
		_rhiContext->clean();
		
		for (int i = 0; i < obj->_uniformBuffers.size(); ++i)
		{
			_rhiContext->clearBuffer(&obj->_uniformBuffers[i], &obj->_uniformBuffersMemory[i]);
		}

		_rhiContext->clearBuffer(&obj->_vertexBuffer, &obj->_vertexDeviceMemory);
		_rhiContext->clearBuffer(&obj->_indexBuffer, &obj->_indexDeviceMemory);
		_rhiContext->clearImage(&obj->_textureSampler, &obj->_textureImageView, &obj->_textureImage, &obj->_textureMemory);
	}

	void Renderer::updateUniformBuffer(uint32_t currentImage)
	{
		static auto startTime = std::chrono::high_resolution_clock::now();

		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		UniformBufferObject ubo{};
		ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.proj = glm::perspective(glm::radians(45.0f), 1920 / (float)1080, 0.1f, 10.0f);
		ubo.proj[1][1] *= -1;

		memcpy(obj->_uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
	}
}