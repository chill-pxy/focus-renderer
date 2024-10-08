#pragma once

#include"NativeWindow.h"

namespace FOCUS
{
	struct WindowSystemCreateInfo
	{
		const char* title;
		uint32_t width;
		uint32_t height;
	};

	class WindowSystem : public NativeWindow
	{
	public:
		WindowSystem() = delete;
		WindowSystem(WindowSystemCreateInfo wsci)
		{
			NativeWindowCreateInfo nwci{};
			nwci.title = wsci.title;
			nwci.width = wsci.width;
			nwci.height = wsci.height;
			nwci.wndproc = this->handleMessage;
			NativeWindow(nwci);
		}

	private:
		LRESULT CALLBACK handleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	};
}