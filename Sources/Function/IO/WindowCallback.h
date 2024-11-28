#pragma once

#include"../Render/RenderSystem.h"
#include"../../Platform/WindowSystem.h"
#include"../../Editor/Overlay.h"

namespace FOCUS
{
	void onRenderCanvasSizeChanged(uint32_t width, uint32_t height)
	{
		if (RenderSystem::getInstance()->_isInitialized)
		{
			RenderSystem::getInstance()->_renderer->_prepared = false;

			EngineUI::getInstance()->_prepared = false;

			RenderSystem::getInstance()->setSwapChainSize(width, height);
		}
	}

	void onWindowDestroy()
	{
		RenderSystem::getInstance()->clean();
	}

	void onWindowClose()
	{
		WindowSystem::getInstance()->close();
	}

	void onWindowMaxSize()
	{
		WindowSystem::getInstance()->setMaxWindow();
	}

	void onWindowMinSize()
	{
		WindowSystem::getInstance()->setMinWindow();
	}

	void onRecoverWindowSize()
	{
		WindowSystem::getInstance()->recoverWindow();
	}
}