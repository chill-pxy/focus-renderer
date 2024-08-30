#pragma once

#include"GLFW/glfw3.h"
#include<volk.h>
#include<vector>

namespace FOCUS
{
    class NativeWindow
    {
    private:
        GLFWwindow* _window;

    public:
        NativeWindow(GLFWwindow* window):_window(window){}

        void cleanup()
        {
            glfwDestroyWindow(_window);

            glfwTerminate();
        }

        void update()
        {
            glfwPollEvents();
        }

        GLFWwindow* getWindowInstance()
        {
            return _window;
        }


        bool checkForClose()
        {
            return glfwWindowShouldClose(_window);
        }
    };
}
