#pragma once

#include<memory>

#include"Renderer.h"
#include"../../Platform/NativeWindow.h"

namespace FOCUS
{
	class RenderSystem
	{
	private:
		std::unique_ptr<Renderer> _renderer;

	public:
		RenderSystem() = delete;
		RenderSystem(NativeWindow* window);

		void initialize();
		void tick();
		void clean();
	};
}