#pragma once

#include<memory>
#include<vector>

#include<Vulkan/VulkanDRHI.h>

namespace FOCUS
{
	class Renderer
	{
	private:
		DRHI::DynamicRHI* _rhiContext;
		std::vector<void*> uniformBuffersMapped;

	public:
		Renderer() = delete;

		Renderer(DRHI::API api, DRHI::PlatformInfo platformCI);
		void updateUniformBuffer(uint32_t currentImage);

		void initialize();
		void draw();
		void clean();
	};
}