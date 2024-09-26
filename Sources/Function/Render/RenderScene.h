#pragma once

#include<memory>

#include"Renderer.h"

namespace FOCUS
{
	class RenderScene
	{
	private:
		std::unique_ptr<Renderer> _renderer;

	public:
		RenderScene() = default;

		void initialize();
		void prepareRenderResources();
	};
}