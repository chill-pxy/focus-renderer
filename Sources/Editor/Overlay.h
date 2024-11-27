#pragma once

#include<Windows.h>
#include<memory>
#include<mutex>

#include<imgui.h>
#include<drhi.h>

namespace FOCUS
{
	class EngineUI
	{
	private:
		DRHI::API      _backend{ DRHI::VULKAN };

		DRHI::DynamicDescriptorPool _descriptorPool{};
		std::vector<DRHI::DynamicDescriptorSet> _descriptorSets{};

		DRHI::DynamicCommandPool _commandPool{};	
		DRHI::DynamicSampler _textureSampler{};

		std::shared_ptr<DRHI::DynamicRHI> _rhi;
		
	public:
		static std::shared_ptr<EngineUI> _instance;
		static std::mutex _mutex;

		std::vector<DRHI::DynamicImage> _viewportImages{};
		std::vector<DRHI::DynamicDeviceMemory> _viewportImageMemorys{};
		std::vector<DRHI::DynamicImageView> _viewportImageViews{};

		std::vector<DRHI::DynamicCommandBuffer> _commandBuffers{};
		
		bool _isEmpty = true;
		bool _prepared{ false };

		uint32_t _viewportWidth{ 0 };
		uint32_t _viewportHeight{ 0 };

	public:
		EngineUI(const EngineUI&) = delete;
		EngineUI& operator=(const EngineUI&) = delete;

		static std::shared_ptr<EngineUI> getInstance()
		{
			if (!_instance)
			{
				std::lock_guard<std::mutex> lock(_mutex);
				if (!_instance)
				{
					_instance.reset(new EngineUI());
				}
			}

			return _instance;
		}

		EngineUI() = default;

		void draw();
		void initialize();
		void tick();
		void recreate();
		void clean();

	private:
		void setStyle();
	};
}