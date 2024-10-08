#pragma once

#include<Windows.h>
#include<vector>

namespace FOCUS
{
    struct NativeWindowCreateInfo
    {
        const char* title;
        uint32_t width;
        uint32_t height;
        WNDPROC wndproc;
    };

    class NativeWindow
    {
    public:
        NativeWindowCreateInfo _createInfo{};
        HWND _hwnd{ NULL };
        MSG _msg{ NULL };
        uint32_t _width{ 0 };
        uint32_t _height{ 0 };

    public:
        NativeWindow() = delete;
        NativeWindow(NativeWindowCreateInfo info) : _createInfo(info), _width(info.width), _height(info.height)
        {
            createWin32Windwow(info.wndproc);
        }

        void initialize();
        bool tick();
        virtual int HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

        HWND getRawWindow()
        {
            return _hwnd;
        }

        uint32_t getWindowWidth()
        {
            return _width;
        }
        
        uint32_t getWindowHeight()
        {
            return _height;
        }

    private:
        void createWin32Windwow(WNDPROC wndproc);

        //LRESULT CALLBACK WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

        //static LRESULT CALLBACK InitialWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
        //    if (Msg == WM_NCCREATE) {
        //        LPCREATESTRUCT create_struct = reinterpret_cast<LPCREATESTRUCT>(lParam);
        //        void* lpCreateParam = create_struct->lpCreateParams;
        //        NativeWindow* this_window = reinterpret_cast<NativeWindow*>(lpCreateParam);
        //        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this_window));
        //        SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&NativeWindow::StaticWndProc));
        //        return this_window->WndProc(hWnd, Msg, wParam, lParam);
        //    }
        //    return DefWindowProc(hWnd, Msg, wParam, lParam);
        //}

        //static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
        //    LONG_PTR user_data = GetWindowLongPtr(hWnd, GWLP_USERDATA);
        //    NativeWindow* this_window = reinterpret_cast<NativeWindow*>(user_data);
        //    return this_window->WndProc(hWnd, Msg, wParam, lParam);
        //}
    };
}
