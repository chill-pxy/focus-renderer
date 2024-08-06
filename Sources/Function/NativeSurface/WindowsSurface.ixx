module;
#include"GLFW/glfw3.h"
export module WindowsSurface;

namespace FOCUS
{
    export class WindowsSurface
    {
    public:
        void init()
        {
            glfwInit();

            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

            _window = glfwCreateWindow(1920, 1080, "FOCUS", nullptr, nullptr);
        }

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

    private:
        GLFWwindow* _window;
    };
}
