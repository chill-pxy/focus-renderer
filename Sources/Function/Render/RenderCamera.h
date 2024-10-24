#pragma once

#include"../../Core/Math.h"

namespace FOCUS
{
	class RenderCamera
	{
	public:
		Vector3 _position{ 0.0f, 0.0f, 0.0f };
		Quaternion _rotation;
		Quaternion _inverseRotation;

		float _zNear{1000.0f,};
		float _zFar{0.1f};

		Vector3 _upAxis;

	public:
		RenderCamera();

		
	};
}