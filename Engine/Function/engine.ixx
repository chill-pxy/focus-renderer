module;
#include<memory>
#include<iostream>
#include<GLFW/glfw3.h>

#include "../Platform/Interface/Context.h"
export module Engine;

import WindowContext;

namespace FOCUS
{
	export class Engine
	{
	public:
		Engine()
		{
			_windowContext = std::make_unique<WindowContext>();
			_platformContext = std::make_unique<Platform::Context>();
			init();
		}

		void init()
		{
			_windowContext->init();
			_platformContext->initialize();
		}

		void run()
		{
			while (!glfwWindowShouldClose(_windowContext->getWindowInstance())) 
			{
				glfwPollEvents();
			}

			_windowContext->cleanup();
		}

	private:
		std::unique_ptr<WindowContext> _windowContext;
		std::unique_ptr<Platform::Context> _platformContext;
	};
}