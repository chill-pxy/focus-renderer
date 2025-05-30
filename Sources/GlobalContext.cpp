#include"GlobalContext.h"

#include<future>

namespace focus
{
	GlobalContext::GlobalContext()
	{
		// init window system
		WindowSystemCreateInfo windowCreateInfo{};
		windowCreateInfo.title = "FOCUS";
		windowCreateInfo.width = 1280;
		windowCreateInfo.height = 720;

		WindowSystemSingleton::getInstance()->initialize(windowCreateInfo);

		// init render system
		RenderSystemCreateInfo renderSystemCreateInfo{};
		renderSystemCreateInfo.window = WindowSystemSingleton::getInstance()->getNativeWindow()->getRawWindow();
		renderSystemCreateInfo.width = WindowSystemSingleton::getInstance()->getWindowWidth();
		renderSystemCreateInfo.height = WindowSystemSingleton::getInstance()->getWindowHeight();

		RenderSystemSingleton::getInstance()->initialize(renderSystemCreateInfo);

		// init ui
		EngineUISingleton::getInstance()->initialize();

		RenderSystemSingleton::getInstance()->_recreateFunc.push_back(std::bind_back(&EngineUI::recreate, EngineUISingleton::getInstance()));
		RenderSystemSingleton::getInstance()->_recreateFunc.push_back(std::bind(&Renderer::recreate, RenderSystemSingleton::getInstance()->_renderer));
		RenderSystemSingleton::getInstance()->build();

		// init fsr
		RenderSystemSingleton::getInstance()->initializeFSR();
	}

	void GlobalContext::tick(bool* running, bool* tempStop)
	{
		while (running)
		{
			_jobComplete = false;
			if (!*running) break;
			
			if (*tempStop == false)
			{
				EngineUISingleton::getInstance()->tick(running);
				RenderSystemSingleton::getInstance()->tick(running);
				WindowSystemSingleton::getInstance()->tick(running);
			}
			_jobComplete = true;
		}

		RenderSystemSingleton::getInstance()->clean();
		WindowSystemSingleton::getInstance()->close();
	}
}