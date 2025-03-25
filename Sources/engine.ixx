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
		}

		void run()
		{
			_globalContext.tick(&_running);
		}
	};
}