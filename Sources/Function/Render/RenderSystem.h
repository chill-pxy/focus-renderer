#pragma once

#include<memory>
#include<mutex>

#include"Renderer.h"
#include"Overlay.h"
#include"../../Platform/NativeWindow.h"

namespace FOCUS
{
	struct RenderSystemCreateInfo
	{
		HWND window;
		uint32_t width;
		uint32_t height;
	};

	class RenderSystem
	{
	private:
		std::shared_ptr<Renderer> _renderer;
		std::unique_ptr<EngineUI> _ui;
		static std::shared_ptr<RenderSystem> _instance;
		static std::mutex _mutex;

		RenderSystem() = default;

	public:
		RenderSystem(const RenderSystem&) = delete;
		RenderSystem& operator=(const RenderSystem&) = delete;

		static std::shared_ptr<RenderSystem> getInstance()
		{
			if (!_instance)
			{
				std::lock_guard<std::mutex> lock(_mutex);
				if (!_instance)
				{
					_instance.reset(new RenderSystem());
				}
			}

			return _instance;
		}

		void initialize(RenderSystemCreateInfo rsci);
		void tick();
		void clean();
		void setViewportSize(uint32_t width, uint32_t height);

		std::shared_ptr<Renderer> getRenderer();
	};
}