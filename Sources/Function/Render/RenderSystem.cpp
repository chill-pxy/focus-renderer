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

		_ui = std::make_unique<EngineUI>(rsci.window);

		_renderer->initialize();

		_ui->initialize();
		_ui->build(_renderer->_rhiContext);
		_ui->preparePipeline(_renderer->_rhiContext);
	}

	void RenderSystem::tick()
	{
		_renderer->_rhiContext->frameOnTick(std::bind(&Renderer::buildCommandBuffer, _renderer));
		_renderer->tick();
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