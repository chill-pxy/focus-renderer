#include<drhi.h>

#include"RenderSystem.h"
#include"RenderScene.h"

namespace FOCUS
{
	void RenderSystem::initialize(RenderSystemCreateInfo rsci)
	{
		DRHI::PlatformInfo platformCI{};
		platformCI.window = rsci.window;
		platformCI.width = rsci.width;
		platformCI.height = rsci.height;

		_renderer = std::make_shared<Renderer>(DRHI::VULKAN, platformCI);

		_renderer->initialize();

		_camera = std::make_shared<RenderCamera>();
	}

	void RenderSystem::tick()
	{
		auto tStart = std::chrono::high_resolution_clock::now();

		_renderer->_rhiContext->frameOnTick(std::bind(&Renderer::buildCommandBuffer, _renderer));
		_renderer->tick(_camera->getViewMatrix());
		_frameCounter++;
		
		auto tEnd = std::chrono::high_resolution_clock::now();
		auto tDiff = std::chrono::duration<double, std::milli>(tEnd - tStart).count();
		
		_frameTimer = (float)tDiff / 1000.0f;

		_camera->handleMovement(_frameTimer);
		
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
	}

	void RenderSystem::clean()
	{
		_renderer->clean();
	}

	void RenderSystem::setViewportSize(uint32_t width, uint32_t height)
	{
		if (_renderer)
		{
			auto vkrhi = static_cast<DRHI::VulkanDRHI*>(_renderer->_rhiContext.get());
			vkrhi->_viewPortWidth = width;
			vkrhi->_viewPortHeight = height;
		}
	}

	std::shared_ptr<Renderer> RenderSystem::getRenderer()
	{
		return _renderer;
	}
}