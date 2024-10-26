#include"RenderCamera.h"

namespace FOCUS
{
	Matrix4 RenderCamera::getViewMatrix()
	{
		return lookAt(_position, _position + _front, _up);
	}

	void RenderCamera::handleMovement(CameraMovement movement, float deltaTime)
	{
		float velocity = _speed * deltaTime;
		if (movement == FORWARD)
			_position += _front * velocity;
		if (movement == BACKWARD)
			_position -= _front * velocity;
		if (movement == LEFT)
			_position -= _right * velocity;
		if (movement == RIGHT)
			_position += _right * velocity;
	}

	void RenderCamera::handleMovement(float xoffset, float yoffset)
	{
		xoffset *= _sensitivity;
		yoffset *= _sensitivity;

		_yaw += xoffset;
		_pitch += yoffset;

		// make sure that when pitch is out of bounds, screen doesn't get flipped

		if (_pitch > 89.0f)
			_pitch = 89.0f;
		if (_pitch < -89.0f)
			_pitch = -89.0f;

		// update Front, Right and Up Vectors using the updated Euler angles
		updateCameraVectors();
	}

	void RenderCamera::handleMovement(float yoffset)
	{
		_zoom -= (float)yoffset;
		if (_zoom < 1.0f)
			_zoom = 1.0f;
		if (_zoom > 45.0f)
			_zoom = 45.0f;
	}

	void RenderCamera::updateCameraVectors()
	{
		Vector3 front{};
		front.x = cos(radians(_yaw)) * cos(radians(_pitch));
		front.y = sin(radians(_pitch));
		front.z = sin(radians(_yaw)) * cos(radians(_pitch));

		_front = normalize(front);
		_right = normalize(cross(_front, _worldUp));
		_up    = normalize(cross(_right, _front));
	}
}