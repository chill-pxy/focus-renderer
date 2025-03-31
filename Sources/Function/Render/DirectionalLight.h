#pragma once

#include"../../Core/Math.h"

namespace focus
{
	class DirectionalLight
	{
	public:
		Vector3 _position{ 1.0, 500.0, 1.0 };
		Vector3 _target{ 1.0, 1.0, 150.0 };
		Vector3 _color{ 1.0, 1.0, 1.0 };
		float _intensity{ 1.0f };
	};
}