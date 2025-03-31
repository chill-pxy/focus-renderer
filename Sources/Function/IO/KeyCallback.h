#pragma once

#include"../Render/RenderCamera.h"

namespace focus
{
	void onKeyDown(RenderCamera* camera, char key)
	{
		switch (key)
		{
		case 'W':
			camera->_forward = true;
			break;
		case 'A':
			camera->_moveleft = true;
			break;
		case 'S':
			camera->_backward = true;
			break;
		case 'D':
			camera->_moveright = true;
			break;
		}
	}

	void onKeyUp(RenderCamera* camera, char key)
	{
		switch (key)
		{
		case 'W':
			camera->_forward = false;
			break;
		case 'A':
			camera->_moveleft = false;
			break;
		case 'S':
			camera->_backward = false;
			break;
		case 'D':
			camera->_moveright = false;
			break;
		}
	}
}