#include<drhi.h>

#include"RenderSystem.h"

#define IMGUI_IMPL_VULKAN_USE_VOLK
#include<imgui.h>
#include<imgui_impl_vulkan.h>
#include<imgui_impl_win32.h>

namespace focus
{
	void RenderSystem::initialize(RenderSystemCreateInfo rsci)
	{
		count++;
		drhi::PlatformInfo platformCI{};
		platformCI.window = rsci.window;
		platformCI.width = rsci.width;
		platformCI.height = rsci.height;

		// initialize renderer
		_renderer = std::make_shared<Renderer>(drhi::VULKAN, platformCI);
		_renderer->initialize();

		// initialize scene
		_scene = std::make_shared<RenderScene>();
		_scene->initialize(_renderer->_rhiContext);

		_isInitialized = true;
	}

	void RenderSystem::tick(bool* running)
	{
		if (!_isInitialized) return; 
		if (!*running) return;

		auto tStart = std::chrono::high_resolution_clock::now();

		// renderer tick
		_renderer->_rhiContext->frameOnTick(_recreateFunc, &_submitCommandBuffers);
		
		// compute time on every frame cost
		_frameCounter++;
		
		auto tEnd = std::chrono::high_resolution_clock::now();
		auto tDiff = std::chrono::duration<double, std::milli>(tEnd - tStart).count();
		
		_frameTimer = (float)tDiff / 1000.0f;

		// scene tick
		_scene->tick(_frameTimer);
		_renderer->_environmentMap->updateUniformBuffer(_scene->_uud);
		//_renderer->buildCommandBuffer();

		// Convert to clamped timer value
		_timer += _timerSpeed * _frameTimer;
		if (_timer > 1.0)
		{
			_timer -= 1.0f;
		}

		float fpsTimer = (float)(std::chrono::duration<double, std::milli>(tEnd - _lastTimestamp).count());
		if (fpsTimer > 1000.0f)
		{
			_lastFPS = static_cast<uint32_t>((float)_frameCounter * (1000.0f / fpsTimer));

			_frameCounter = 0;
			_lastTimestamp = tEnd;
		}
		_tPrevEnd = tEnd;

		*running = true;
	}

	void RenderSystem::clean()
	{
		_scene->clean(_renderer->_rhiContext);
		_renderer->clean();
		_isInitialized = false;
	}

	void RenderSystem::setViewportSize(uint32_t width, uint32_t height)
	{
		if (_renderer)
		{
			auto vkrhi = static_cast<drhi::VulkanDRHI*>(_renderer->_rhiContext.get());
			vkrhi->_viewPortWidth = width;
			vkrhi->_viewPortHeight = height;
		}
	}

	void RenderSystem::setSwapChainSize(uint32_t width, uint32_t height)
	{
		if (_renderer)
		{
			auto vkrhi = static_cast<drhi::VulkanDRHI*>(_renderer->_rhiContext.get());
			vkrhi->_swapChainExtent.width = width;
			vkrhi->_swapChainExtent.height = height;
		}
	}

	void RenderSystem::build()
	{
		_renderer->buildAndSubmit(&_scene->_group, &_scene->_sceneCommandBuffers, &_scene->_sceneCommandPool);
	}

	std::shared_ptr<Renderer> RenderSystem::getRenderer()
	{
		return _renderer;
	}
}