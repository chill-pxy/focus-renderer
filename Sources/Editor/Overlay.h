#pragma once
#ifdef FOCUS_EXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif

#include<Windows.h>
#include<memory>
#include<mutex>
#include<filesystem>

#include<imgui.h>
#include<drhi.h>

#include"../Core/Math.h"
#include"../Function/Render/RenderResource.h"
#include"../Core/Path.h"

namespace focus
{
	enum FileType
	{
		NONE,
		FOLDER,
		MODEL,	
		IMAGE
	};

	class EngineUI
	{
	private:
		drhi::API      _backend{ drhi::VULKAN };

		drhi::DynamicDescriptorPool _descriptorPool{};
		std::vector<drhi::DynamicDescriptorSet> _descriptorSets{};
	
		drhi::DynamicSampler _textureSampler{};

		std::shared_ptr<drhi::DynamicRHI> _rhi;

		std::filesystem::path _browserPath;
		std::string _selectedFile{"UnSelected"};
		FileType _selectedType{ FileType::NONE };

	public:
		std::vector<drhi::DynamicImage> _viewportImages{};
		std::vector<drhi::DynamicDeviceMemory> _viewportImageMemorys{};
		std::vector<drhi::DynamicImageView> _viewportImageViews{};

		drhi::DynamicImage _viewportDepthImage{};
		drhi::DynamicDeviceMemory _viewportDepthImageMemory{};
		drhi::DynamicImageView _viewportDepthImageView{};

		drhi::DynamicCommandPool _commandPool{};
		std::vector<drhi::DynamicCommandBuffer> _commandBuffers{};
		
		bool _isEmpty = true;
		bool _prepared{ false };
		bool _isMaxSize{ false };

		uint32_t _viewportWidth{ 0 };
		uint32_t _viewportHeight{ 0 };

		bool _sceneOpen{ true };
		bool _propertyOpen{ true };
		bool _infoOpen{ true };
		bool _viewportOpen{ true };
		bool _filebrowserOpen{ true };

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
		void showInfoUI();
		void showFileBrowswerUI();
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