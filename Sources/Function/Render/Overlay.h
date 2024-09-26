#pragma once

#include<imgui.h>

namespace FOCUS
{
	class EngineUI
	{
	public:
		float _scale{ 1.0f };

	public:

		EngineUI() = default;

		void initialize();
	};
}