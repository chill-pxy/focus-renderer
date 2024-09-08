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
    };

    class NativeWindow
    {
    private:
        NativeWindowCreateInfo _createInfo{};
        HWND _hwnd{NULL};
        MSG _msg{NULL};

    public:
        NativeWindow() = delete;
        NativeWindow(NativeWindowCreateInfo info) : _createInfo(info) 
        {
            createWin32Windwow(); 
        }

        void initialize();
        bool tick();

        HWND getRawWindow()
        {
            return _hwnd;
        }

    private:
        void createWin32Windwow();
    };
}
