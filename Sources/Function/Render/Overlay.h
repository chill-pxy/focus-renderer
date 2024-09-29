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

	public:

		EngineUI() = delete;
		EngineUI(HWND window, Renderer* renderer);

		void initialize();
	};
}