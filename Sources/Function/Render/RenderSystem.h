#pragma once

#include<memory>

#include"Renderer.h"
#include"Overlay.h"
#include"../../Platform/NativeWindow.h"

namespace FOCUS
{
	struct RenderSystemCreateInfo
	{
		HWND window;
		uint32_t width;
		uint32_t height;
	};

	class RenderSystem
	{
	private:
		std::shared_ptr<Renderer> _renderer;
		std::unique_ptr<EngineUI> _ui;

	public:
		RenderSystem() = delete;
		RenderSystem(RenderSystemCreateInfo rsci);

		void initialize();
		void tick();
		void clean();
	};
}