#pragma once

#include"../../Core/Math.h"

namespace FOCUS
{
	enum CameraMovement {
		NONE,
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT
	};

	class RenderCamera
	{
	public:
		Vector3 _rotation{};
		Vector3 _position{};
		Vector3 _front{};
		Vector3 _up{};
		Vector3 _right{};
		Vector3 _worldUp{};
		Vector2 _mousePosition{};

		float _yaw;
		float _pitch;
		
		float _speed;
		float _rotateSpeed;
		float _zoom;

		bool _isRotate = false;

		Matrix4 _view;

		CameraMovement _state{ NONE };

	public:
		RenderCamera()
		{
			_position = { 0.0f, 0.0f, 1.0f };
			_front = { 0.0f,0.0f,1.0f };
			_up = { 0.0f, 1.0f,0.0f };

			_yaw = -90.0f;
			_pitch = 0.0f;
			_speed = 2.5f;
			_rotateSpeed = 1.0f;
			_zoom = 45.0f;

			updateCameraVectors();
			updateViewMatrix();
		}

		Matrix4 getViewMatrix();
		void handleMovement(float deltaTime);
		void updateViewMatrix();
		void makeRotate(Vector3 delta);

	private:
		void updateCameraVectors();

	};
}