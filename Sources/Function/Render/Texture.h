#pragma once

#include<stb_image.h>

#include<stdint.h>

namespace FOCUS
{
	class Texture
	{
	public:
		stbi_uc* _pixels;
		uint32_t _width;
		uint32_t _height;
		uint32_t _channels;

	public:
		Texture() = default;
	};

	Texture* loadTexture(const char* texturePath);
}