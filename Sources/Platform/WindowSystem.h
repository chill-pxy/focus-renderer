#pragma once
#ifdef FOCUS_EXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif

#include<memory>
#include<mutex>

#include"NativeWindow.h"

namespace FOCUS
{
	struct WindowSystemCreateInfo
	{
		const char* title;
		uint32_t width;
		uint32_t height;
	};

	class WindowSystem
	{
	private:
		std::shared_ptr<NativeWindow> _nativeWindow;
		WindowSystemCreateInfo _wsci{};

	public:
		void tick(bool* running);
		void initialize(WindowSystemCreateInfo wsci);
		void setWindowSize(uint32_t width, uint32_t height);
		void close();
		void setMaxWindow();
		void setMinWindow();
		void recoverWindow();
		uint32_t getWindowWidth();
		uint32_t getWindowHeight();
		NativeWindow* getNativeWindow();	
	};

	class DLL_EXPORT WindowSystemSingleton
	{
	public:
		static WindowSystem* getInstance()
		{
			static WindowSystem instance;
			return &instance;
		}

		WindowSystemSingleton(WindowSystem&&) = delete;
		WindowSystemSingleton(const WindowSystem&) = delete;
		void operator= (const WindowSystem&) = delete;

	protected:
		WindowSystemSingleton() = default;
		virtual ~WindowSystemSingleton() = default;
	};
}