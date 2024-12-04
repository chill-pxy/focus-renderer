#pragma once

#include"../../Core/Math.h"

namespace FOCUS
{
	class DirectionalLight
	{
	public:
		Vector3 _direction{ 0.0, 0.0, 0.0 };
		Vector3 _color{ 1.0, 1.0, 1.0 };
		float _intensity{ 1.0f };
	};
}