#pragma once
#ifdef FOCUS_EXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif

#include<Windows.h>
#include<memory>
#include<mutex>

#include<imgui.h>
#include<drhi.h>

#include"../Core/Math.h"
#include"../Function/Render/RenderResource.h"

namespace FOCUS
{
	class EngineUI
	{
	private:
		DRHI::API      _backend{ DRHI::VULKAN };

		DRHI::DynamicDescriptorPool _descriptorPool{};
		std::vector<DRHI::DynamicDescriptorSet> _descriptorSets{};
	
		DRHI::DynamicSampler _textureSampler{};

		std::shared_ptr<DRHI::DynamicRHI> _rhi;
		
	public:
		std::vector<DRHI::DynamicImage> _viewportImages{};
		std::vector<DRHI::DynamicDeviceMemory> _viewportImageMemorys{};
		std::vector<DRHI::DynamicImageView> _viewportImageViews{};

		DRHI::DynamicImage _viewportDepthImage{};
		DRHI::DynamicDeviceMemory _viewportDepthImageMemory{};
		DRHI::DynamicImageView _viewportDepthImageView{};

		DRHI::DynamicCommandPool _commandPool{};
		std::vector<DRHI::DynamicCommandBuffer> _commandBuffers{};
		
		bool _isEmpty = true;
		bool _prepared{ false };
		bool _isMaxSize{ false };

		uint32_t _viewportWidth{ 0 };
		uint32_t _viewportHeight{ 0 };

		//property
		std::shared_ptr<RenderResource> _currentObj{nullptr};

	public:
		EngineUI() = default;

		void draw();
		void initialize();
		void tick(bool* running);
		void recreate();
		void clean();
		Vector2 getViewportSize();

	private:
		void setStyle();
		void showSceneUI();
		void showPropertyUI();
		void showViewPortUI();
		void showMenu(bool* running);
	};

	class DLL_EXPORT EngineUISingleton
	{
	public:
		static EngineUI* getInstance()
		{
			static EngineUI instance;
			return &instance;
		}

		EngineUISingleton(EngineUI&&) = delete;
		EngineUISingleton(const EngineUI&) = delete;
		void operator= (const EngineUI&) = delete;

	protected:
		EngineUISingleton() = default;
		virtual ~EngineUISingleton() = default;
	};
}