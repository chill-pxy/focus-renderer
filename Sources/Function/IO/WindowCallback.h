#pragma once

#include"../Render/RenderSystem.h"
#include"../../Platform/WindowSystem.h"
#include"../../Editor/Overlay.h"

namespace focus
{
	void onRenderCanvasSizeChanged(uint32_t width, uint32_t height)
	{
		if (RenderSystemSingleton::getInstance()->_isInitialized)
		{
			RenderSystemSingleton::getInstance()->_renderer->_prepared = false;

			EngineUISingleton::getInstance()->_prepared = false;

			RenderSystemSingleton::getInstance()->setSwapChainSize(width, height);
		}
	}

	void onWindowDestroy()
	{
		RenderSystemSingleton::getInstance()->clean();
	}

	void onWindowClose()
	{
		WindowSystemSingleton::getInstance()->close();
	}

	void onWindowMaxSize()
	{
		WindowSystemSingleton::getInstance()->setMaxWindow();
	}

	void onWindowMinSize()
	{
		WindowSystemSingleton::getInstance()->setMinWindow();
	}

	void onRecoverWindowSize()
	{
		WindowSystemSingleton::getInstance()->recoverWindow();
	}
}