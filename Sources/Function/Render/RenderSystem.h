#pragma once

#include<memory>
#include<mutex>

#include"Renderer.h"
#include"RenderScene.h"
#include"RenderCamera.h"

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
	public:
		static std::shared_ptr<RenderSystem> _instance;
		static std::mutex _mutex;

		std::shared_ptr<Renderer> _renderer;
		std::shared_ptr<RenderScene> _scene;
		std::shared_ptr<EngineUI> _ui;

		std::vector<std::function<void()>> _recreateFunc;

		uint32_t _frameCounter = 0;
		uint32_t _lastFPS = 0;
		float    _frameTimer = 1.0f;
		float    _timer = 0.0f;
		float    _timerSpeed = 0.25f;

		std::chrono::time_point<std::chrono::high_resolution_clock> _lastTimestamp;
		std::chrono::time_point<std::chrono::high_resolution_clock>	_tPrevEnd;

		bool _isInitialized{ false };

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