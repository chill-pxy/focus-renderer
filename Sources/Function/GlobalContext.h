#pragma once

#include<memory>

#include"../Platform/NativeWindow.h";
#include"Render/RenderSystem.h"

namespace FOCUS
{
	class GlobalContext
	{
	public:
		std::unique_ptr<RenderSystem> _renderSytem;
		std::unique_ptr<NativeWindow> _window;

	public:
		GlobalContext()
		{
			NativeWindowCreateInfo windowCreateInfo =
			{
				"FOCUS",
				1920,
				1080
			};

			_window = std::make_unique<NativeWindow>(windowCreateInfo);

			_renderSytem = std::make_unique<RenderSystem>(_window.get());
		}

		void initialize()
		{
			_window->initialize();
			_renderSytem->initialize();
		}
	};
}