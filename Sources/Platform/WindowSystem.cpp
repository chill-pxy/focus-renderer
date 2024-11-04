#include"imgui.h"
#include"imgui_impl_win32.h"

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

		switch (uMsg)
		{





		case WM_CLOSE:
			DestroyWindow(hWnd);
			break;






		case WM_DESTROY:
			PostQuitMessage(0);
			break;






		case WM_SIZE:
		{
			UINT width = LOWORD(lParam);
			UINT height = HIWORD(lParam);

			WindowSystem::getInstance()->setWindowSize(width, height);
			onRenderCanvasSizeChanged(width, height);
			break;
		}





		case WM_KEYDOWN:
		{
			wchar_t key = (wchar_t)wParam;
			onKeyDown(RenderSystem::getInstance()->_scene->_camera.get(), key);
			break;
		}





		case WM_KEYUP:
		{
			wchar_t key = (wchar_t)wParam;
			onKeyUp(RenderSystem::getInstance()->_scene->_camera.get(), key);
			break;
		}


		
		case WM_RBUTTONDOWN:
		{
			float mouseX = (float)LOWORD(lParam);
			float mouseY = (float)HIWORD(lParam);

			onRightMouseButtonDown(RenderSystem::getInstance()->_scene->_camera.get(), mouseX, mouseY);

			break;
		}




		case WM_RBUTTONUP:
		{
			onRightMouseButtonUp(RenderSystem::getInstance()->_scene->_camera.get());
			break;
		}




		case WM_MOUSEMOVE:
		{
			float x = LOWORD(lParam);
			float y = HIWORD(lParam);

			onMouseMove(RenderSystem::getInstance()->_scene->_camera.get(), x, y);
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