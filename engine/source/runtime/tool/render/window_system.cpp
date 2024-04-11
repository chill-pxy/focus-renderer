//
// Created by pxy on 2024/4/10.
//
module;
#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
module window_system;

namespace FOCUS
{
    void WindowSystem::init()
    {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        GLFWwindow* window = glfwCreateWindow(800, 800, "FOCUS Rendering Engine", nullptr, nullptr);

        while (!glfwWindowShouldClose(window))
        {
            glfwPollEvents();
        }

        glfwDestroyWindow(window);

        glfwTerminate();
    }

}

