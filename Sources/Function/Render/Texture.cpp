#include"Texture.h"

namespace FOCUS
{
	Texture* loadTexture(const char* texturePath)
	{
		Texture* t = new Texture();
		t->_pixels = stbi_load(texturePath, (int*)(&t->_width), (int*)(&t->_height), (int*)(&t->_channels), STBI_rgb_alpha);
		return t;
	}
}