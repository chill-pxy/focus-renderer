#pragma once
#ifdef FOCUS_EXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif

#include<memory>
#include<mutex>

#include"Renderer.h"
#include"RenderScene.h"
#include"RenderCamera.h"

namespace focus
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
		uint32_t                   _maxThread = 1;
		drhi::DynamicCommandBuffer _priCmdbuf;
		drhi::DynamicCommandPool   _priCmdpool;

	public:
		std::shared_ptr<Renderer>    _renderer;
		std::shared_ptr<RenderScene> _scene;

		std::vector<std::function<void()>>      _recreateFunc;
		std::vector<drhi::DynamicCommandBuffer> _submitCommandBuffers;

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
		void initialize(RenderSystemCreateInfo rsci);
		void tick(bool* running);
		void clean();
		void build();
		void setViewportSize(uint32_t width, uint32_t height);
		void setSwapChainSize(uint32_t width, uint32_t height);

		std::shared_ptr<Renderer> getRenderer();
	};

	class DLL_EXPORT RenderSystemSingleton
	{
	public:
		static RenderSystem* getInstance()
		{
			static RenderSystem instance;
			return &instance;
		}

		RenderSystemSingleton(RenderSystem&&) = delete;
		RenderSystemSingleton(const RenderSystem&) = delete;
		void operator= (const RenderSystem&) = delete;

	protected:
		RenderSystemSingleton() = default;
		virtual ~RenderSystemSingleton() = default;
	};
}