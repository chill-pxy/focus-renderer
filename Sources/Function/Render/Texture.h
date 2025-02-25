#pragma once

#include<stb_image.h>
#include<ktx.h>
#include<drhi.h>

#include<memory>
#include<stdint.h>
#include<vector>


namespace FOCUS
{
	class Texture
	{
	public:
		stbi_uc* _pixels;
		uint32_t _width;
		uint32_t _height;
		uint32_t _channels;

		//ktx data
		ktx_uint8_t* _ktxData;
		ktx_size_t _ktxSize;
		uint32_t _mipLevels;
		std::vector<size_t> _offsets;
		std::vector<DRHI::DynamicExtent2D> _texSizes;

	public:
		Texture() = default;
	};

	std::shared_ptr<Texture> loadTexture(const char* texturePath);
	std::shared_ptr<Texture> loadCubeTexture(const char* texturePath);
}