module;
#define GLFW_INCLUDE_VULKAN
#include"GLFW/glfw3.h"
export module window_context;

namespace FOCUS
{
    export class WindowContext
    {
    public:
        WindowContext() = default;

        void init()
        {
            glfwInit();

            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            _window = glfwCreateWindow(800, 800, "FOCUS", nullptr, nullptr);

            while (!glfwWindowShouldClose(_window))
            {
                glfwPollEvents();
            }

            glfwDestroyWindow(_window);

            glfwTerminate();
        }

        GLFWwindow* getWindowInstance()
        {
            return _window;
        }

    private:
        GLFWwindow* _window;
    };
}
