#include"Texture.h"

namespace FOCUS
{
	std::shared_ptr<Texture> loadTexture(const char* texturePath)
	{
		std::shared_ptr<Texture>  t = std::make_shared<Texture>();
		t->_pixels = stbi_load(texturePath, (int*)(&t->_width), (int*)(&t->_height), (int*)(&t->_channels), STBI_rgb_alpha);
		return t;
	}
}