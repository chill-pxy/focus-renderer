#pragma once

#include"../Render/RenderSystem.h"

namespace FOCUS
{
	void onRenderCanvasSizeChanged(uint32_t width, uint32_t height)
	{
		if (RenderSystem::getInstance()->_isInitialized)
		{
			RenderSystem::getInstance()->_ui->_prepared = false;
			RenderSystem::getInstance()->_renderer->_prepared = false;
		}

		RenderSystem::getInstance()->setViewportSize(width, height);
		
	}

	void onWindowDestroy()
	{
		RenderSystem::getInstance()->clean();
	}
}