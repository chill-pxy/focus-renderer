#pragma once

#include<memory>

#include"Platform/WindowSystem.h"
#include"Function/Render/RenderSystem.h"
#include"Editor/Overlay.h"

namespace focus
{
	class GlobalContext
	{
	public:
		bool _jobComplete{ false };

		GlobalContext();
		void tick(bool* running, bool* tempStop);
	};
}