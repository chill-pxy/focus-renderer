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
		GlobalContext();
		void tick(bool* running);
	};
}