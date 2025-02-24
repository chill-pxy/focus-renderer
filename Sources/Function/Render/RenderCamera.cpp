#include"RenderCamera.h"

namespace FOCUS
{
	Matrix4 RenderCamera::getViewMatrix()
	{
		return _view;
	}

	void RenderCamera::handleMovement(float deltaTime)
	{
		_front.x = -cos(radians(_rotation.x)) * sin(radians(_rotation.y));
		_front.y = sin(radians(_rotation.x));
		_front.z = cos(radians(_rotation.x)) * cos(radians(_rotation.y));
		_front = normalize(_front);

		float velocity = _speed * deltaTime;
		if (_forward)
			_position += _front * velocity;
		if (_backward)
			_position -= _front * velocity;
		if (_moveleft)
			_position -= normalize(cross(_front, Vector3(0.0f, 1.0f, 0.0f))) * velocity;
		if (_moveright)
			_position += normalize(cross(_front, Vector3(0.0f, 1.0f, 0.0f))) * velocity;

		updateViewMatrix();
	}

	void RenderCamera::updateViewMatrix()
	{
		Matrix4 currentMatrix = _view;
		Matrix4 transM;
		Matrix4 rotM = Matrix4(1.0f);

		float filp = _filpY ? -1.0f : 1.0f;

		rotM = rotate(rotM, radians(_rotation.x * filp), Vector3(1.0f, 0.0f, 0.0f));
		rotM = rotate(rotM, radians(_rotation.y), Vector3(0.0f, 1.0f, 0.0f));
		rotM = rotate(rotM, radians(_rotation.z), Vector3(0.0f, 0.0f, 1.0f));

		Vector3 translation = _position;
		translation.y *= filp;
		transM = translate(Matrix4(1.0f), translation);

		_view = rotM * transM;
	}

	void RenderCamera::makeRotate(Vector3 delta)
	{
		_rotation += delta;
		if (_rotation.x >= 90) _rotation.x = 90.0;
		if (_rotation.x <= -90) _rotation.x = -90.0;
		updateViewMatrix();		
	}
}