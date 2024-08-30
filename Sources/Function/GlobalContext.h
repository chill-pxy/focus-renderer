#pragma once

#include<memory>

#include"Render/Renderer.h";
#include"NativeWindow.h";

namespace FOCUS
{
	class GlobalContext
	{
	private:
		std::shared_ptr<Renderer> _renderer;
		std::shared_ptr<NativeWindow> _surface;

	public:
		GlobalContext()
		{
			DRHI::VulkanGlfwWindowCreateInfo windowCreateInfo =
			{
				"FOCUS",
				1920,
				1080
			};

			RendererCreateInfo renderCreateInfo =
			{
				DRHI::API::VULKAN,
				windowCreateInfo
			};

			_renderer = std::make_shared<Renderer>(renderCreateInfo);
			_renderer->initialize();

			_surface = std::make_shared<NativeWindow>(_renderer->getRendererWindow());
		}

		Renderer* getRenderer()
		{
			return _renderer.get();
		}

		NativeWindow* getNativeWindow()
		{
			return _surface.get();
		}
	};
}