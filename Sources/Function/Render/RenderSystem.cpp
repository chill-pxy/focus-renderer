#include<drhi.h>

#include"RenderSystem.h"
#include"RenderScene.h"

namespace FOCUS
{
	RenderSystem::RenderSystem(RenderSystemCreateInfo rsci)
	{
		DRHI::PlatformInfo platformCI{};
		platformCI.window = rsci.window;
		platformCI.width = rsci.width;
		platformCI.height = rsci.height;

		_renderer = std::make_shared<Renderer>(DRHI::VULKAN, platformCI);

		_ui = std::make_unique<EngineUI>(rsci.window, _renderer.get());
	}

	void RenderSystem::initialize()
	{
		_renderer->initialize();
		_ui->initialize();
	}

	void RenderSystem::tick()
	{
		_renderer->draw();
	}

	void RenderSystem::clean()
	{
		_renderer->clean();
	}
}