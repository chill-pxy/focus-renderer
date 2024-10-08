#include"WindowSystem.h"

namespace FOCUS
{
	LRESULT CALLBACK WindowSystem::handleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (uMsg == WM_CLOSE)
		{
			DestroyWindow(hWnd);
		}
		if (uMsg == WM_DESTROY)
		{
			PostQuitMessage(0);
		}
		if (uMsg == WM_SIZE)
		{
			UINT width = LOWORD(lParam);
			UINT height = HIWORD(lParam);
			_width = width;
			//_height = height;
		}

		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}