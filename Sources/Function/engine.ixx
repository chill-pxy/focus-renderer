module;
#include<memory>
#include<iostream>

#include<Context.h>
export module Engine;

import WindowsSurface;

namespace FOCUS
{
	export class Engine
	{
	public:
		Engine()
		{
			_windowContext = std::make_unique<WindowsSurface>();
			_windowContext->init();

			DRHI::ContextCreatInfo info = {
				API::VULKAN,
				_windowContext->getWindowInstance(),
				_windowContext->getExtensions()
			};

			_platformContext = std::make_unique<DRHI::Context>(info);
			

			init();
		}

		void init()
		{
			_platformContext->initialize();
		}

		void run()
		{
			while (!_windowContext->checkForClose()) 
			{
				_windowContext->update();
			}

			_windowContext->cleanup();
		}

	private:
		std::unique_ptr<DRHI::Context> _platformContext;
		std::unique_ptr<WindowsSurface> _windowContext;
	};
}