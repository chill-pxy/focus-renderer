#pragma once

#include"../../Core/Math.h"

namespace focus
{
	class RenderCamera
	{
	public:
		Vector3 _rotation{};
		Vector3 _position{};
		Vector3 _front{};
		Vector2 _mousePosition{};
		Matrix4 _view;
		Matrix4 _proj;

		float _speed{ 0.f };
		float _rotateSpeed{ 0.f };

		uint32_t _viewportWidth{ 0 };
		uint32_t _viewportHeight{ 0 };

		float _jitterX{ 0.f };
		float _jitterY{ 0.f };

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
			_speed = 75.0f;
			_rotateSpeed = 0.01f;

			updateViewMatrix();
			updateProjMatrix();
		}

		Matrix4 getViewMatrix();
		Matrix4 getProjMatrix();
		void handleMovement(float deltaTime);
		void updateViewMatrix();
		void updateProjMatrix();
		void makeRotate(Vector3 delta);
		void applyJitter();
	};
}