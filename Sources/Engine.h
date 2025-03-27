#pragma once
#ifdef FOCUS_EXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif

#include<memory>
#include<iostream>

#include"GlobalContext.h"
#include"Core/Path.h"

namespace FOCUS
{
	class Engine
	{
	private:
		GlobalContext _globalContext{};
		bool _running = false;
		bool _tempStop = false;

	public:
		Engine() = default;

		void initialize()
		{
			_running = true;
			_tempStop = false;
		}

		void run()
		{
			_globalContext.tick(&_running, &_tempStop);
		}

		void stop()
		{
			_tempStop = true;
		}

		void start()
		{
			_tempStop = false;
		}

		bool getJobState()
		{
			return _globalContext._jobComplete;
		}
	};

	class DLL_EXPORT EngineSingleton
	{
	public:
		static Engine* getInstance()
		{
			static Engine instance;
			return &instance;
		}

		EngineSingleton(Engine&&) = delete;
		EngineSingleton(const Engine&) = delete;
		void operator= (const Engine&) = delete;

	protected:
		EngineSingleton() = default;
		virtual ~EngineSingleton() = default;
	};
}
