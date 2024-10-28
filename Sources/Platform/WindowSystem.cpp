#include"imgui.h"
#include"imgui_impl_win32.h"

#include"WindowSystem.h"
#include"../Function/IO/KeyCallback.h"

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

			switch (key)
			{
			case 'W':
				std::cout << "camera w" << std::endl;
				RenderSystem::getInstance()->_camera->_state = CameraMovement::FORWARD;
				break;
			case 'A':
				std::cout << "camera a" << std::endl;
				RenderSystem::getInstance()->_camera->_state = CameraMovement::LEFT;
				break;
			case 'S':
				std::cout << "camera s" << std::endl;
				RenderSystem::getInstance()->_camera->_state = CameraMovement::BACKWARD;
				break;
			case 'D':
				std::cout << "camera d" << std::endl;
				RenderSystem::getInstance()->_camera->_state = CameraMovement::RIGHT;
				break;
			}

			break;
		}





		case WM_KEYUP:
		{
			wchar_t key = (wchar_t)wParam;

			RenderSystem::getInstance()->_camera->_state = CameraMovement::NONE;

			break;
		}


		
		case WM_LBUTTONDOWN:
		{
			RenderSystem::getInstance()->_camera->_isRotate = true;
			RenderSystem::getInstance()->_camera->_mousePosition = Vector2((float)LOWORD(lParam), (float)HIWORD(lParam));
			break;
		}




		case WM_LBUTTONUP:
		{
			RenderSystem::getInstance()->_camera->_isRotate = false;
			break;
		}




		case WM_MOUSEMOVE:
		{
			float x = LOWORD(lParam);
			float y = HIWORD(lParam);
			int32_t dx = (int32_t)RenderSystem::getInstance()->_camera->_mousePosition.x - x;
			int32_t dy = (int32_t)RenderSystem::getInstance()->_camera->_mousePosition.y - y;
			RenderSystem::getInstance()->_camera->makeRotate(Vector3(dy * 1.0f, -dx * 1.0f, 0.0f));

			RenderSystem::getInstance()->_camera->_mousePosition = Vector2(x, y);

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