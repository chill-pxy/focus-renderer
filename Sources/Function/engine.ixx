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
			_surfaceContext = std::make_unique<WindowsSurface>();
			_platformContext = std::make_unique<DRHI::Context>();

			init();
		}

		void init()
		{
			_surfaceContext->init();
			_platformContext->initialize();
		}

		void run()
		{
			while (!_surfaceContext->checkForClose()) 
			{
				_surfaceContext->update();
			}

			_surfaceContext->cleanup();
		}

	private:
		std::unique_ptr<WindowsSurface> _surfaceContext;
		std::unique_ptr<DRHI::Context> _platformContext;
	};
}