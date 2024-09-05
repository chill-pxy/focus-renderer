module;
#include<memory>
#include<iostream>

#include"GlobalContext.h"
export module Engine;

namespace FOCUS
{
	export class Engine
	{
	private:
		std::unique_ptr<GlobalContext> _globalContext;
		bool _running = false;

	public:
		Engine()
		{
			_globalContext = std::make_unique<GlobalContext>();
		}

		void initialize()
		{
			_running = true;
		}

		void run()
		{
			while (_running)
			{
				_running = _globalContext->getNativeWindow()->tick();
			}
		}
	};
}