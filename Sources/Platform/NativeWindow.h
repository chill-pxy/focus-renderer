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
        uint32_t _width{ 0 };
        uint32_t _height{ 0 };

    public:
        NativeWindow() = delete;
        NativeWindow(NativeWindowCreateInfo info) : _createInfo(info), _width(info.width), _height(info.height)
        {
            createWin32Windwow(); 
        }

        void initialize();
        bool tick();

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
        void createWin32Windwow();
    };
}
