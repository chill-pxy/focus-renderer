#pragma once

#include<stb_image.h>

#include<memory>
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

	std::shared_ptr<Texture> loadTexture(const char* texturePath);
}