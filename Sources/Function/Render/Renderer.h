#pragma once

#include<memory>
#include<vector>

#include<drhi.h>
#include<imgui.h>

namespace FOCUS
{
	class Renderer
	{
	private:
		DRHI::DynamicRHI* _rhiContext;

		DRHI::DynamicBuffer vertexBuffer;
		DRHI::DynamicDeviceMemory vertexDeviceMemory;

		DRHI::DynamicBuffer indexBuffer;
		DRHI::DynamicDeviceMemory indexDeviceMemory;

		std::vector<void*> uniformBuffersMapped;
		std::vector<DRHI::DynamicBuffer> uniformBuffers;
		std::vector<DRHI::DynamicDeviceMemory> uniformBuffersMemory;

		DRHI::DynamicImage textureImage;
		DRHI::DynamicImageView textureImageView;
		DRHI::DynamicSampler textureSampler;
		DRHI::DynamicDeviceMemory textureMemory;

		DRHI::DynamicPipeline modelPipeline;
		DRHI::DynamicPipelineLayout modelPipelineLayout;

		DRHI::DynamicDescriptorSet modelDescriptorSet;
		DRHI::DynamicDescriptorSetLayout modelDescriptorSetLayout;

		DRHI::DynamicPipeline uiPipeline;

		DRHI::API _api;

	public:
		Renderer() = delete;

		Renderer(DRHI::API api, DRHI::PlatformInfo platformCI);
		void updateUniformBuffer(uint32_t currentImage);

		void initialize();
		void draw();
		void clean();
	};
}