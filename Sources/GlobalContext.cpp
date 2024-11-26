#include"GlobalContext.h"

namespace FOCUS
{
	GlobalContext::GlobalContext()
	{
		WindowSystemCreateInfo windowCreateInfo{};
		windowCreateInfo.title = "FOCUS";
		windowCreateInfo.width = 1280;
		windowCreateInfo.height = 720;

		WindowSystem::getInstance()->initialize(windowCreateInfo);

		RenderSystemCreateInfo renderSystemCreateInfo{};
		renderSystemCreateInfo.window = WindowSystem::getInstance()->getNativeWindow()->getRawWindow();
		renderSystemCreateInfo.width = WindowSystem::getInstance()->getWindowWidth();
		renderSystemCreateInfo.height = WindowSystem::getInstance()->getWindowHeight();

		RenderSystem::getInstance()->initialize(renderSystemCreateInfo);
	}

	void GlobalContext::tick(bool* running)
	{
		while (running)
		{
			*running = WindowSystem::getInstance()->tick();
			*running = RenderSystem::getInstance()->tick();
		}

		RenderSystem::getInstance()->clean();
	}
}