#pragma once

#include<memory>
#include<mutex>

#include"NativeWindow.h"

namespace FOCUS
{
	struct WindowSystemCreateInfo
	{
		const char* title;
		uint32_t width;
		uint32_t height;
	};

	class WindowSystem
	{
	private:
		std::shared_ptr<NativeWindow> _nativeWindow;
		WindowSystemCreateInfo _wsci{};
		static std::shared_ptr<WindowSystem> _instance;
		static std::mutex _mutex;

		WindowSystem() = default;

	public:
		WindowSystem(const WindowSystem&) = delete;
		WindowSystem& operator=(const WindowSystem&) = delete;

		static std::shared_ptr<WindowSystem> getInstance()
		{
			if (!_instance) 
			{
				std::lock_guard<std::mutex> lock(_mutex);
				if (!_instance) 
				{
					_instance.reset(new WindowSystem());
				}
			}

			return _instance;
		}

		bool tick();
		void initialize(WindowSystemCreateInfo wsci);
		void setWindowSize(uint32_t width, uint32_t height);
		uint32_t getWindowWidth();
		uint32_t getWindowHeight();
		NativeWindow* getNativeWindow();	
	};

	std::shared_ptr<WindowSystem> WindowSystem::_instance = nullptr;
	std::mutex WindowSystem::_mutex;
}