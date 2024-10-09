#pragma once

#include<memory>

#include"../Platform/WindowSystem.h"
#include"Render/RenderSystem.h"

namespace FOCUS
{
	class GlobalContext
	{
	public:
		std::unique_ptr<RenderSystem> _renderSytem;

	public:
		GlobalContext()
		{
			WindowSystemCreateInfo windowCreateInfo{};
			windowCreateInfo.title = "FOCUS";
			windowCreateInfo.width = 1920;
			windowCreateInfo.height = 1080;

			WindowSystem::getInstance()->initialize(windowCreateInfo);

			RenderSystemCreateInfo renderSystemCreateInfo{};
			renderSystemCreateInfo.window = WindowSystem::getInstance()->getNativeWindow()->getRawWindow();
			renderSystemCreateInfo.width = WindowSystem::getInstance()->getWindowWidth();
			renderSystemCreateInfo.height = WindowSystem::getInstance()->getWindowHeight();

			_renderSytem = std::make_unique<RenderSystem>(renderSystemCreateInfo);
		}

		void initialize()
		{
			_renderSytem->initialize();
		}
	};
}