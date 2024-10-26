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
		_renderer->tick();
		_frameCounter++;
		
		auto tEnd = std::chrono::high_resolution_clock::now();
		auto tDiff = std::chrono::duration<double, std::milli>(tEnd - tStart).count();
		
		_frameTimer = (float)tDiff / 1000.0f;

		_camera->handleMovement();
		
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