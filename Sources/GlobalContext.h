#pragma once

#include<memory>

#include"Platform/WindowSystem.h"
#include"Function/Render/RenderSystem.h"
#include"Editor/Overlay.h"

namespace FOCUS
{
	std::shared_ptr<RenderSystem> RenderSystem::_instance = nullptr;
	std::mutex RenderSystem::_mutex;

	std::shared_ptr<WindowSystem> WindowSystem::_instance = nullptr;
	std::mutex WindowSystem::_mutex;

	std::shared_ptr<EngineUI> EngineUI::_instance = nullptr;
	std::mutex EngineUI::_mutex;

	class GlobalContext
	{
	public:
		GlobalContext();
		void tick(bool* running);
	};
}