#pragma once

#include"../Render/RenderCamera.h"
namespace focus
{
	void onRightMouseButtonDown(RenderCamera* camera, float x, float y)
	{
		camera->_isRotate = true;
		camera->_mousePosition = Vector2(x, y);
	}

	void onRightMouseButtonUp(RenderCamera* camera)
	{
		camera->_isRotate = false;
	}

	void onMouseMove(RenderCamera* camera, float x, float y)
	{
		if (camera->_isRotate)
		{
			int32_t dx = (int32_t)camera->_mousePosition.x - x;
			int32_t dy = (int32_t)camera->_mousePosition.y - y;
			camera->makeRotate(Vector3(dy * 1.0f, -dx * 1.0f, 0.0f));

			camera->_mousePosition = Vector2(x, y);
		}
	}
}