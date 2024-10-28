#include"RenderCamera.h"

namespace FOCUS
{
	Matrix4 RenderCamera::getViewMatrix()
	{
		return _view;
	}

	void RenderCamera::handleMovement(float deltaTime)
	{
		float velocity = _speed * deltaTime;
		if (_state == FORWARD)
			_position += _front * velocity;
		if (_state == BACKWARD)
			_position -= _front * velocity;
		if (_state == LEFT)
			_position -= normalize(cross(_front, Vector3(0.0f, 1.0f, 0.0f))) * velocity;
		if (_state == RIGHT)
			_position += normalize(cross(_front, Vector3(0.0f, 1.0f, 0.0f))) * velocity;

		updateViewMatrix();
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

	void RenderCamera::updateViewMatrix()
	{
		Matrix4 currentMatrix = _view;
		Matrix4 transM;

		Vector3 translation = _position;
		transM = translate(Matrix4(1.0f), -translation);

		_view = transM;
	}
}