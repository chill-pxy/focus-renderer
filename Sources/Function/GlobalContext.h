#pragma once

#include<memory>

//#include"Render/Renderer.h";
#include"../Platform/NativeWindow.h";

namespace FOCUS
{
	class GlobalContext
	{
	private:
		//std::shared_ptr<Renderer> _renderer;
		std::shared_ptr<NativeWindow> _window;

	public:
		GlobalContext()
		{
			//DRHI::VulkanGlfwWindowCreateInfo windowCreateInfo =
			//{
			//	"FOCUS",
			//	1920,
			//	1080
			//};

			//RendererCreateInfo renderCreateInfo =
			//{
			//	DRHI::API::VULKAN,
			//	windowCreateInfo
			//};

			//_renderer = std::make_shared<Renderer>(renderCreateInfo);
			//_renderer->initialize();

			NativeWindowCreateInfo windowCreateInfo =
			{
				"FOCUS",
				1920,
				1080
			};

			_window = std::make_shared<NativeWindow>(windowCreateInfo);
			_window->initialize();
		}

		//Renderer* getRenderer()
		//{
			//return _renderer.get();
		//}

		NativeWindow* getNativeWindow()
		{
			return _window.get();
		}
	};
}