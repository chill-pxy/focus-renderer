#pragma once

#include<Windows.h>
#include<memory>

#include<imgui.h>

#include"RenderResource.h"
#include"RenderScene.h"

namespace FOCUS
{
	class EngineUI
	{
	private:
		HWND           _window;
		DRHI::API      _backend{ DRHI::VULKAN };

		DRHI::DynamicDescriptorPool _descriptorPool{};
		std::vector<DRHI::DynamicDescriptorSet> _descriptorSets{};

		DRHI::DynamicCommandPool _commandPool{};	
		DRHI::DynamicSampler _textureSampler{};

		std::shared_ptr<DRHI::DynamicRHI> _rhi;

		
		
	public:
		std::vector<DRHI::DynamicImage> _viewportImages{};
		std::vector<DRHI::DynamicDeviceMemory> _viewportImageMemorys{};
		std::vector<DRHI::DynamicImageView> _viewportImageViews{};

		std::vector<DRHI::DynamicCommandBuffer> _commandBuffers{};
		
		bool _isEmpty = true;
		bool _prepared{ false };

		uint32_t _viewportWidth{ 0 };
		uint32_t _viewportHeight{ 0 };

	public:

		EngineUI() = delete;
		EngineUI(HWND window);

		void draw(std::shared_ptr<DRHI::DynamicRHI> rhi);
		void initialize(std::shared_ptr<DRHI::DynamicRHI> rhi);
		void tick(uint32_t fps, std::shared_ptr<RenderScene> scene, std::shared_ptr<DRHI::DynamicRHI> rhi);
		void recreate();
		void clean();

	private:
		void setStyle();
	};
}