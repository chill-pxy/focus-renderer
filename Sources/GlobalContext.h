#pragma once

#include<memory>

#include"Platform/WindowSystem.h"
#include"Function/Render/RenderSystem.h"
#include"Editor/Overlay.h"

namespace FOCUS
{
	class GlobalContext
	{
	public:
		GlobalContext();
		void tick(bool* running);
	};
}