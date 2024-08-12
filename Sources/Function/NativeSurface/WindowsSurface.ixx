module;
#include"GLFW/glfw3.h"
#include<volk.h>
#include<vector>
export module WindowsSurface;

namespace FOCUS
{
    export class WindowsSurface
    {
    public:
        void init()
        {
            //glfwInit();

            //glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            //glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

            //_window = glfwCreateWindow(1920, 1080, "FOCUS", nullptr, nullptr);

            glfwInit();
            uint32_t glfwExtensionCount = 0;
            const char** glfwExtensions;
            glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

            _extensions = std::vector<const char*>(glfwExtensions, glfwExtensions + glfwExtensionCount);

            //if (enableValidationLayers)
            //{
                _extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            //}

            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            if (!glfwVulkanSupported())
            {
                printf("GLFW: Vulkan Not Supported\n");
            }

            _window = glfwCreateWindow(100, 100, "focus", nullptr, nullptr);
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

        std::vector<const char*> getExtensions()
        {
            return _extensions;
        }


        bool checkForClose()
        {
            return glfwWindowShouldClose(_window);
        }

    private:
        GLFWwindow* _window;
        std::vector<const char*> _extensions;
    };
}
