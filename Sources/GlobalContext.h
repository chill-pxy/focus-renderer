#pragma once

#include<memory>

#include"Platform/WindowSystem.h"
#include"Function/Render/RenderSystem.h"

namespace FOCUS
{
	class GlobalContext
	{
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

			RenderSystem::getInstance()->initialize(renderSystemCreateInfo);
		}
	};
}