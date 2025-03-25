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
	}

	void GlobalContext::tick(bool* running)
	{
		while (running)
		{
			if (!*running) break;
			
			EngineUISingleton::getInstance()->tick(running);
			if (!EngineUISingleton::getInstance()->_isEmpty)
			{
				RenderSystemSingleton::getInstance()->_submitCommandBuffers.clear();
				RenderSystemSingleton::getInstance()->_submitCommandBuffers.push_back(
					RenderSystemSingleton::getInstance()->_renderer->_defferedCommandBuffer);

				RenderSystemSingleton::getInstance()->_submitCommandBuffers.push_back(
					RenderSystemSingleton::getInstance()->_renderer->_shadowCommandBuffers[RenderSystemSingleton::getInstance()->_renderer->_rhiContext->getCurrentFrame()]);

				RenderSystemSingleton::getInstance()->_submitCommandBuffers.push_back(
					RenderSystemSingleton::getInstance()->_scene->_sceneCommandBuffers[RenderSystemSingleton::getInstance()->_renderer->_rhiContext->getCurrentFrame()]);

				for (uint32_t i = 0; i < EngineUISingleton::getInstance()->_commandBuffers.size(); ++i)
				{
					RenderSystemSingleton::getInstance()->_submitCommandBuffers.push_back(EngineUISingleton::getInstance()->_commandBuffers[i]);
				}
			}

			RenderSystemSingleton::getInstance()->tick(running);
			WindowSystemSingleton::getInstance()->tick(running);
		}

		RenderSystemSingleton::getInstance()->clean();
		WindowSystemSingleton::getInstance()->close();
	}
}