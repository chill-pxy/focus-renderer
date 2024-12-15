#pragma once

#include"../../Core/Math.h"

namespace FOCUS
{
	class RenderCamera
	{
	public:
		Vector3 _rotation{};
		Vector3 _position{};
		Vector3 _front{};
		Vector2 _mousePosition{};
		Matrix4 _view;

		float _speed;
		float _rotateSpeed;

		bool _isRotate = false;
		bool _filpY    = true;
		
		bool _forward   = false;
		bool _backward  = false;
		bool _moveleft  = false;
		bool _moveright = false;

	public:
		RenderCamera()
		{
			_position = { 0.0f, 0.0f, 1.0f };
			_front = { 1.0f,0.0f,0.0f };
			_speed = 25.0f;
			_rotateSpeed = 0.01f;

			updateViewMatrix();
		}

		Matrix4 getViewMatrix();
		void handleMovement(float deltaTime);
		void updateViewMatrix();
		void makeRotate(Vector3 delta);

	};
}