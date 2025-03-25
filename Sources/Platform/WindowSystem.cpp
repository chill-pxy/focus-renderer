#include"imgui.h"
#include"imgui_impl_win32.h"

#include <Windows.h>
#include <windowsx.h>
#include <Dwmapi.h>

#include"WindowSystem.h"
#include"../Function/IO/WindowCallback.h"
#include"../Function/IO/KeyCallback.h"
#include"../Function/IO/MouseCallback.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace FOCUS
{
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	//--------------------------- Key Callback function ----------------------------
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	static LRESULT handleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
			return true;

		static RECT border_thickness;

		switch (uMsg)
		{
		case WM_CREATE:
		{
			//find border thickness
			SetRectEmpty(&border_thickness);
			if (GetWindowLongPtr(hWnd, GWL_STYLE) & WS_THICKFRAME)
			{
				AdjustWindowRectEx(&border_thickness, GetWindowLongPtr(hWnd, GWL_STYLE) & ~WS_CAPTION, FALSE, NULL);
				border_thickness.left *= -1;
				border_thickness.top *= -1;
			}
			else if (GetWindowLongPtr(hWnd, GWL_STYLE) & WS_BORDER)
			{
				SetRect(&border_thickness, 1, 1, 1, 1);
			}

			MARGINS margins = { 0 };
			DwmExtendFrameIntoClientArea(hWnd, &margins);
			SetWindowPos(hWnd, NULL, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
			
			HICON hIcon = (HICON)LoadImage(NULL,
				TEXT("../../../Asset/Images/focus.ico"), 
				IMAGE_ICON, 
				0, 0, 
				LR_LOADFROMFILE | LR_DEFAULTSIZE);
			SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
			SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
			//DestroyIcon(hIcon);

			break;
		}






		case WM_NCCALCSIZE:
			if (lParam)
			{
				NCCALCSIZE_PARAMS* sz = (NCCALCSIZE_PARAMS*)lParam;
				sz->rgrc[0].left += border_thickness.left;
				sz->rgrc[0].right -= border_thickness.right;
				sz->rgrc[0].bottom -= border_thickness.bottom;
				return 0;
			}
			break;







		case WM_NCHITTEST:
		{
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			ScreenToClient(hWnd, &pt);
			RECT rc;
			GetClientRect(hWnd, &rc);
			enum { left = 1, top = 2, right = 4, bottom = 8, caption = 16 };
			int hit = 0;
			if (pt.x < border_thickness.left) hit |= left;
			if (pt.x > rc.right - border_thickness.right) hit |= right;
			if (pt.y < border_thickness.top) hit |= top;
			if (pt.y > rc.bottom - border_thickness.bottom) hit |= bottom;

			// set 30 px to resize window
			if (pt.y > border_thickness.top && pt.y < border_thickness.top + 10) hit |= caption;

			if (hit & top && hit & left) return HTTOPLEFT;
			if (hit & top && hit & right) return HTTOPRIGHT;
			if (hit & bottom && hit & left) return HTBOTTOMLEFT;
			if (hit & bottom && hit & right) return HTBOTTOMRIGHT;
			if (hit & left) return HTLEFT;
			if (hit & top) return HTTOP;
			if (hit & right) return HTRIGHT;
			if (hit & bottom) return HTBOTTOM;
			if (hit & caption) return HTCAPTION;

			return HTCLIENT;
		}





		case WM_CLOSE:
			onWindowDestroy();
			DestroyWindow(hWnd);
			break;






		case WM_DESTROY:
			PostQuitMessage(0);
			break;






		case WM_SIZE:
		{
			UINT width = LOWORD(lParam);
			UINT height = HIWORD(lParam);

			WindowSystemSingleton::getInstance()->setWindowSize(width, height);
			onRenderCanvasSizeChanged(width, height);
			break;
		}





		case WM_KEYDOWN:
		{
			wchar_t key = (wchar_t)wParam;
			onKeyDown(RenderSystemSingleton::getInstance()->_scene->_camera.get(), key);
			break;
		}





		case WM_KEYUP:
		{
			wchar_t key = (wchar_t)wParam;
			onKeyUp(RenderSystemSingleton::getInstance()->_scene->_camera.get(), key);
			break;
		}


		
		case WM_RBUTTONDOWN:
		{
			float mouseX = (float)LOWORD(lParam);
			float mouseY = (float)HIWORD(lParam);

			onRightMouseButtonDown(RenderSystemSingleton::getInstance()->_scene->_camera.get(), mouseX, mouseY);

			break;
		}




		case WM_RBUTTONUP:
		{
			onRightMouseButtonUp(RenderSystemSingleton::getInstance()->_scene->_camera.get());
			break;
		}




		case WM_MOUSEMOVE:
		{
			float x = LOWORD(lParam);
			float y = HIWORD(lParam);

			onMouseMove(RenderSystemSingleton::getInstance()->_scene->_camera.get(), x, y);
			break;
		}

		}

		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}







	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	//-------------------------------- class function ------------------------------
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	void WindowSystem::tick(bool* running)
	{
		if (!*running) return;
		_nativeWindow->tick(running);
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

	void WindowSystem::close()
	{
		DestroyWindow(_nativeWindow->_hwnd);
	}

	void WindowSystem::setMaxWindow()
	{
		ShowWindow(_nativeWindow->_hwnd, SW_MAXIMIZE);
	}

	void WindowSystem::setMinWindow()
	{
		ShowWindow(_nativeWindow->_hwnd, SW_MINIMIZE);
	}

	void WindowSystem::recoverWindow()
	{
		ShowWindow(_nativeWindow->_hwnd, SW_RESTORE);
	}

	uint32_t WindowSystem::getWindowWidth()
	{
		_nativeWindow->_hwnd = GetForegroundWindow();

		RECT rect;
		int width = 0;
		if (GetWindowRect(_nativeWindow->_hwnd, &rect)) {
			width = rect.right - rect.left;
		}
		return width;
	}

	uint32_t WindowSystem::getWindowHeight()
	{
		_nativeWindow->_hwnd = GetForegroundWindow();

		RECT rect;
		int height = 0;
		if (GetWindowRect(_nativeWindow->_hwnd, &rect)) {
			height = rect.bottom - rect.top;
		}
		return height;
	}

	NativeWindow* WindowSystem::getNativeWindow()
	{
		return _nativeWindow.get();
	}
}