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
			while (!_globalContext->getNativeWindow()->checkForClose())
			{
				_globalContext->getNativeWindow()->update();
			}

			_globalContext->getNativeWindow()->cleanup();
		}
	};
}