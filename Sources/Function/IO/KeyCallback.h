#pragma once

#include"../Render/RenderSystem.h"

namespace FOCUS
{
	void onRenderCanvasSizeChanged(uint32_t width, uint32_t height)
	{
		RenderSystem::getInstance()->setViewportSize(width, height);
	}

	
}