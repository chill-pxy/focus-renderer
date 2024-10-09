#include"WindowSystem.h"

namespace FOCUS
{
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	//--------------------------- Key Callback function ----------------------------
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	static LRESULT handleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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

			WindowSystem::getInstance()->setWindowSize(width, height);
		}

		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}







	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	//-------------------------------- class function ------------------------------
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	bool WindowSystem::tick()
	{
		return _nativeWindow->tick();
	}

	void WindowSystem::initialize(WindowSystemCreateInfo wsci)
	{
		_wsci = wsci;

		NativeWindowCreateInfo nwci{};
		nwci.title = _wsci.title;
		nwci.width = _wsci.width;
		nwci.height = _wsci.height;
		nwci.wndproc = handleMessage;

		_nativeWindow = std::make_shared<NativeWindow>(nwci);
	}

	void WindowSystem::setWindowSize(uint32_t width, uint32_t height)
	{
		if (_nativeWindow)
		{
			_nativeWindow->_width = width;
			_nativeWindow->_height = height;
		}
	}

	uint32_t WindowSystem::getWindowWidth()
	{
		return _nativeWindow->_width;
	}

	uint32_t WindowSystem::getWindowHeight()
	{
		return _nativeWindow->_height;
	}

	NativeWindow* WindowSystem::getNativeWindow()
	{
		return _nativeWindow.get();
	}
}