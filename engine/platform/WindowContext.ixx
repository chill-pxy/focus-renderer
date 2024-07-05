module;
#define GLFW_INCLUDE_VULKAN
#include"GLFW/glfw3.h"
export module window_context;

namespace FOCUS
{
    export class WindowContext
    {
    public:
        void init()
        {
            glfwInit();

            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

            _window = glfwCreateWindow(800, 800, "FOCUS", nullptr, nullptr);
        }

        void cleanup()
        {
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
