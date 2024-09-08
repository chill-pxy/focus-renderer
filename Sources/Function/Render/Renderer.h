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

	public:
		Renderer() = delete;

		Renderer(DRHI::API api, DRHI::PlatformInfo platformCI);

		void initialize();
		void draw();
		void clean();
	};
}