#include<drhi.h>

#include"RenderSystem.h"

namespace FOCUS
{
	RenderSystem::RenderSystem(NativeWindow* window)
	{
		DRHI::PlatformInfo platformCI{};
		platformCI.window = window->getRawWindow();
		platformCI.width = window->getWindowWidth();
		platformCI.height = window->getWindowHeight();

		_renderer = std::make_unique<Renderer>(DRHI::VULKAN, platformCI);
	}

	void RenderSystem::initialize()
	{
		_renderer->initialize();
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