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
		GlobalContext _globalContext{};
		bool _running = false;

	public:
		Engine() = default;

		void initialize()
		{
			_running = true;
			_globalContext.initialize();
		}

		void run()
		{
			while (_running)
			{
				_running = _globalContext._window->tick();
				_globalContext._renderSytem->tick();
			}

			_globalContext._renderSytem->clean();
		}
	};
}