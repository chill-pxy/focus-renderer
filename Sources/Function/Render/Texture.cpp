#include"Texture.h"

namespace focus
{
	std::shared_ptr<Texture> loadTexture(const char* texturePath)
	{
		std::shared_ptr<Texture>  t = std::make_shared<Texture>();
		t->_pixels = stbi_load(texturePath, (int*)(&t->_width), (int*)(&t->_height), (int*)(&t->_channels), STBI_rgb_alpha);
		return t;
	}

	std::shared_ptr<Texture> loadCubeTexture(const char* texturePath)
	{
		std::shared_ptr<Texture>  t = std::make_shared<Texture>();
		ktxTexture* ktxTexture{};
		ktxResult result = ktxTexture_CreateFromNamedFile(texturePath, KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &ktxTexture);
		if (result == KTX_SUCCESS)
		{
			ktx_uint8_t* ktxTextureData = ktxTexture_GetData(ktxTexture);
			ktx_size_t ktxTextureSize = ktxTexture_GetSize(ktxTexture);

			t->_channels = 4;
			t->_width = ktxTexture->baseWidth;
			t->_height = ktxTexture->baseHeight;
			t->_ktxData = ktxTextureData;
			t->_ktxSize = ktxTextureSize;
			t->_mipLevels = ktxTexture->numLevels;

			for (uint32_t face = 0; face < 6; ++face)
			{
				for (uint32_t level = 0; level < t->_mipLevels; ++level)
				{
					drhi::DynamicExtent2D extent{};
					extent.width = ktxTexture->baseWidth >> level;
					extent.height = ktxTexture->baseHeight >> level;
					
					t->_texSizes.push_back(extent);

					ktx_size_t offset;
					ktxTexture_GetImageOffset(ktxTexture, level, 0, face, &offset);
					t->_offsets.push_back(offset);
				}
			}
		}

		return t;
	}
}