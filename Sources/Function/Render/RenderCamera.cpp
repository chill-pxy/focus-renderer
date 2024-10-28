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
		Matrix4 rotM = Matrix4(1.0f);

		if (_isRotate)
		{
			rotM = rotate(rotM, radians(_rotation.x * 1.0f), Vector3(1.0f, 0.0f, 0.0f));
			rotM = rotate(rotM, radians(_rotation.y), Vector3(0.0f, 1.0f, 0.0f));
			rotM = rotate(rotM, radians(_rotation.z), Vector3(0.0f, 0.0f, 1.0f));
		}

		Vector3 translation = _position;
		transM = translate(Matrix4(1.0f), -translation);

		_view = rotM * transM;
	}

	void RenderCamera::makeRotate(Vector3 delta)
	{
		_rotation += delta;
		updateViewMatrix();
	}
}