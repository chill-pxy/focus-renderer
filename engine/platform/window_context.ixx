module;
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
            GLFWwindow* window = glfwCreateWindow(800, 800, "FOCUS", nullptr, nullptr);

            while (!glfwWindowShouldClose(window))
            {
                glfwPollEvents();
            }

            glfwDestroyWindow(window);

            glfwTerminate();
        }
    };
}
