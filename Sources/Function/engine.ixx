module;
#include<memory>
#include<iostream>
export module Engine;

import GlobalContext;

namespace FOCUS
{
	export class Engine
	{
	private:
		std::unique_ptr<GlobalContext> _globalContext;

	public:
		Engine()
		{
			_globalContext = std::make_unique<GlobalContext>();
		}

		void initialize()
		{
			
		}

		void run()
		{
			while (!_globalContext->getSurface()->checkForClose()) 
			{
				_globalContext->getSurface()->update();
			}

			_globalContext->getSurface()->cleanup();
		}
	};
}