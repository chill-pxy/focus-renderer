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
		uint32_t       _drawCommandCount{ 0 };

		DRHI::API                   _backend{ DRHI::VULKAN };

		DRHI::DynamicDescriptorPool _descriptorPool{};
		std::vector<VkDescriptorSet> _descriptorSets{};
		
		std::vector<DRHI::DynamicImage> _viewportImages{};
		std::vector<DRHI::DynamicDeviceMemory> _viewportImageMemorys{};
		std::vector<DRHI::DynamicImageView> _viewportImageViews{};

		DRHI::DynamicCommandPool _commandPool{};	

		VkSampler _textureSampler{};

		std::shared_ptr<DRHI::DynamicRHI> _rhi;
		
	public:
		std::vector<DRHI::DynamicCommandBuffer> _commandBuffers{};
		bool _isEmpty = true;

	public:

		EngineUI() = delete;
		EngineUI(HWND window);

		void draw(std::shared_ptr<DRHI::DynamicRHI> rhi);
		void initialize(std::shared_ptr<DRHI::DynamicRHI> rhi);
		void tick(uint32_t fps, std::shared_ptr<RenderScene> scene, std::shared_ptr<DRHI::DynamicRHI> rhi);
		bool needUpdate();
		void recreate();
	};
}