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
			NativeWindowCreateInfo nativeWindowCreateInfo = {
				1920,
				1080,
				"FOCUS"
			};
			_surface = std::make_shared<NativeWindow>();
			_surface->initialize(nativeWindowCreateInfo);

			RendererCreateInfo rendererCreateInfo = { 
				_surface->getWindowInstance(), 
				_surface->getExtensions()
			};
			_renderer = std::make_shared<Renderer>(rendererCreateInfo);
			_renderer->initialize();
		}

		Renderer* getRenderer()
		{
			return _renderer.get();
		}

		NativeWindow* getSurface()
		{
			return _surface.get();
		}
	};
}