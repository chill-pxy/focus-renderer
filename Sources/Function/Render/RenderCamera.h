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
		Vector3 _position{};
		Vector3 _front{};
		Vector3 _up{};
		Vector3 _right{};
		Vector3 _worldUp{};
		
		float _yaw;
		float _pitch;
		
		float _speed;
		float _sensitivity;
		float _zoom;

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
			_sensitivity = 0.1f;
			_zoom = 45.0f;

			updateCameraVectors();
			updateViewMatrix();
		}

		Matrix4 getViewMatrix();
		void handleMovement(float deltaTime);
		void handleMovement(float xoffset, float yoffset);
		void updateViewMatrix();
		//void handleMovement(float yoffset);

	private:
		void updateCameraVectors();

	};
}