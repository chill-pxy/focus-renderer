#include"Renderer.h"
#include"RenderResources.h"
#include"Mesh.h"

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
			_rhiContext = new DRHI::VulkanDRHI(rhiCI);
			break;
		}
	}

	void Renderer::initialize()
	{
		_rhiContext->initialize();

		DRHI::PipelineCreateInfo pci = {};
		pci.vertexShader = "../../../Shaders/model_vertex.spv";
		pci.fragmentShader = "../../../Shaders/model_fragment.spv";

		dynamic_cast<DRHI::VulkanDRHI*>(_rhiContext)->createPipeline(pci);
		
		const char* modelPath = "../../../Asset/Models/viking_room.obj";
		
		//std::vector<Vertex> vertices;
		//std::vector<uint32_t> indices;
		Mesh obj = RenderResources::loadModel(modelPath);

		//create vertex buffer
		DRHI::DynamicBuffer vertexBuffer;
		DRHI::DynamicDeviceMemory vertexDeviceMemory;
		auto vertexBufferSize = sizeof(obj.vertices[0]) * obj.vertices.size();

		_rhiContext->createDynamicBuffer(&vertexBuffer, &vertexDeviceMemory, vertexBufferSize, obj.vertices.data(), "VertexBuffer");

		//create index buffer
		DRHI::DynamicBuffer indexBuffer;
		DRHI::DynamicDeviceMemory indexDeviceMemory;
		auto indexBufferSize = sizeof(obj.indices[0]) * obj.indices.size();

		_rhiContext->createDynamicBuffer(&indexBuffer, &indexDeviceMemory, indexBufferSize, obj.indices.data(), "IndexBuffer");

		//create uniform buffer
		std::vector<DRHI::DynamicBuffer> uniformBuffers;
		std::vector<DRHI::DynamicDeviceMemory> uniformBuffersMemory;
		//std::vector<void*> uniformBuffersMapped;

		_rhiContext->createUniformBuffer(&uniformBuffers, &uniformBuffersMemory, &uniformBuffersMapped, sizeof(UniformBufferObject));

		//texture loading
		int textureWidth, textureHeight, textureChannels;
		stbi_uc* pixels = RenderResources::loadTexture("../../../Asset/Models/viking_room.png", &textureWidth, &textureHeight, &textureChannels);
		if (!pixels)
		{
			throw std::runtime_error("failed to load texture image!");
		}

		//binding sampler and image view
		DRHI::DynamicImage textureImage;
		DRHI::DynamicImageView textureImageView;
		DRHI::DynamicSampler textureSampler;
		DRHI::DynamicDeviceMemory textureMemory;
		_rhiContext->createTextureImage(&textureImage, &textureMemory, textureWidth, textureHeight, textureChannels, pixels);
		_rhiContext->createImageView(&textureImageView, &textureImage);
		_rhiContext->createTextureSampler(&textureSampler);

		_rhiContext->createDescriptorSets(&uniformBuffers, sizeof(UniformBufferObject), textureImageView, textureSampler);

		_rhiContext->prepareCommandBuffer(&vertexBuffer, &indexBuffer, static_cast<uint32_t>(obj.indices.size()));
	}

	void Renderer::draw()
	{
		_rhiContext->frameOnTick();
		updateUniformBuffer(_rhiContext->getCurrentBuffer());
	}

	void Renderer::clean()
	{
		_rhiContext->clean();
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

		memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
	}
}