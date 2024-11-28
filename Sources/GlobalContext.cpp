#include"GlobalContext.h"

namespace FOCUS
{
	GlobalContext::GlobalContext()
	{
		// init window system
		WindowSystemCreateInfo windowCreateInfo{};
		windowCreateInfo.title = "FOCUS";
		windowCreateInfo.width = 1280;
		windowCreateInfo.height = 720;

		WindowSystem::getInstance()->initialize(windowCreateInfo);

		// init render system
		RenderSystemCreateInfo renderSystemCreateInfo{};
		renderSystemCreateInfo.window = WindowSystem::getInstance()->getNativeWindow()->getRawWindow();
		renderSystemCreateInfo.width = WindowSystem::getInstance()->getWindowWidth();
		renderSystemCreateInfo.height = WindowSystem::getInstance()->getWindowHeight();

		RenderSystem::getInstance()->initialize(renderSystemCreateInfo);

		// init ui
		EngineUI::getInstance()->initialize();


		RenderSystem::getInstance()->_recreateFunc.push_back(std::bind_back(&EngineUI::recreate, EngineUI::getInstance()));
		RenderSystem::getInstance()->_recreateFunc.push_back(std::bind(&Renderer::recreate, RenderSystem::getInstance()->_renderer));
		RenderSystem::getInstance()->build();
	}

	void GlobalContext::tick(bool* running)
	{
		while (running)
		{
			if (!*running) break;
			EngineUI::getInstance()->tick(running);
			RenderSystem::getInstance()->tick(running);
			WindowSystem::getInstance()->tick(running);
		}

		RenderSystem::getInstance()->clean();
		WindowSystem::getInstance()->close();
	}
}