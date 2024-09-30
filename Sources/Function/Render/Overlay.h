#pragma once

#include<Windows.h>
#include<memory>

#include"Renderer.h"

namespace FOCUS
{
	class EngineUI
	{
	private:
		Renderer* _renderer;
		HWND _window;

		float _scale{ 1.0f };
		bool _show_demo_window = true;
		bool _show_another_window = false;
		

	public:

		EngineUI() = delete;
		EngineUI(HWND window, Renderer* renderer);

		void initialize();
		void tick();
	};
}