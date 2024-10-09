#pragma once

#include<memory>

#include"Platform/WindowSystem.h"
#include"Function/Render/RenderSystem.h"

namespace FOCUS
{
	std::shared_ptr<RenderSystem> RenderSystem::_instance = nullptr;
	std::mutex RenderSystem::_mutex;

	std::shared_ptr<WindowSystem> WindowSystem::_instance = nullptr;
	std::mutex WindowSystem::_mutex;

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