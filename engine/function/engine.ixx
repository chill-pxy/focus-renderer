module;
#include<memory>
#include<iostream>
#include<GLFW/glfw3.h>
export module Engine;

import WindowContext;
import VulkanRHI;

namespace FOCUS
{
	export class Engine
	{
	public:
		Engine()
		{
			_window_context = std::make_unique<WindowContext>();
			_vulkan_rhi = std::make_unique<VulkanRHI>();

			init();
		}

		void init()
		{
			_window_context->init();
			_vulkan_rhi->init(_window_context->getWindowInstance());
		}

		void run()
		{
			while (!glfwWindowShouldClose(_window_context->getWindowInstance())) 
			{
				glfwPollEvents();
				_vulkan_rhi->drawFrame();
			}

			_vulkan_rhi->cleanup();
			_window_context->cleanup();
		}

	private:
		std::unique_ptr<WindowContext> _window_context;
		std::unique_ptr<VulkanRHI> _vulkan_rhi;
	};
}