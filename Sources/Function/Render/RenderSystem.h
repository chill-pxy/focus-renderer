#pragma once

#include<memory>

#include"Renderer.h"
#include"Overlay.h"
#include"../../Platform/NativeWindow.h"

namespace FOCUS
{
	class RenderSystem
	{
	private:
		std::shared_ptr<Renderer> _renderer;
		std::unique_ptr<EngineUI> _ui;

	public:
		RenderSystem() = delete;
		RenderSystem(NativeWindow* window);

		void initialize();
		void tick();
		void clean();
	};
}