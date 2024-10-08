#include"NativeWindow.h"

namespace FOCUS
{
	void NativeWindow::createWin32Windwow(WNDPROC wndproc)
	{
		WNDCLASSEX wndclass = {};
		wndclass.cbSize = sizeof(wndclass);
		wndclass.hInstance = GetModuleHandleA(NULL);
		wndclass.lpszClassName = _createInfo.title;
		wndclass.lpfnWndProc = wndproc;
		RegisterClassEx(&wndclass);

		RECT winRect{0};
		winRect.left = 0;
		winRect.right = _createInfo.width;
		winRect.top = 0;
		winRect.bottom = _createInfo.height;

		AdjustWindowRectEx(&winRect, WS_OVERLAPPEDWINDOW, NULL, NULL);
		
		_hwnd = CreateWindowEx(
			WS_EX_ACCEPTFILES, 
			_createInfo.title,
			_createInfo.title,
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			winRect.right - winRect.left,
			winRect.bottom - winRect.top,
			NULL,
			NULL, 
			GetModuleHandleA(NULL),
			NULL
		);

		ShowWindow(_hwnd, SW_SHOW);
	}

	void NativeWindow::initialize()
	{
		
	}

	bool NativeWindow::tick()
	{
		if (PeekMessageW(&_msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&_msg);
			DispatchMessageW(&_msg);
			if (_msg.message == WM_QUIT)
				return false;
		}

		return true;
	}

	int NativeWindow::HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		return 0;
	}
}