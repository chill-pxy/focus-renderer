#include<drhi.h>

#include"RenderSystem.h"
#include"RenderScene.h"

namespace FOCUS
{
	RenderSystem::RenderSystem(NativeWindow* window)
	{
		DRHI::PlatformInfo platformCI{};
		platformCI.window = window->getRawWindow();
		platformCI.width = window->getWindowWidth();
		platformCI.height = window->getWindowHeight();

		_renderer = std::make_shared<Renderer>(DRHI::VULKAN, platformCI);

		_ui = std::make_unique<EngineUI>(window->getRawWindow(), _renderer.get());
	}

	void RenderSystem::initialize()
	{
		_renderer->initialize();
		_ui->initialize();
	}

	void RenderSystem::tick()
	{
		_renderer->draw();
		_ui->tick();
	}

	void RenderSystem::clean()
	{
		_renderer->clean();
	}
}