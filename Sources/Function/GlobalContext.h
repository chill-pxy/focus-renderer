#pragma once

#include<memory>

#include"../Platform/WindowSystem.h"
#include"Render/RenderSystem.h"

namespace FOCUS
{
	class GlobalContext
	{
	public:
		std::unique_ptr<RenderSystem> _renderSytem;
		std::unique_ptr<WindowSystem> _window;

	public:
		GlobalContext()
		{
			WindowSystemCreateInfo windowCreateInfo =
			{
				"FOCUS",
				1920,
				1080
			};

			_window = std::make_unique<WindowSystem>(windowCreateInfo);

			_renderSytem = std::make_unique<RenderSystem>(_window.get());
		}

		void initialize()
		{
			_window->initialize();
			_renderSytem->initialize();
		}
	};
}