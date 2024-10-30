#pragma once

#include<memory>

#include"Texture.h"

namespace FOCUS
{
	class BlinnPhongMaterial
	{
	public:
		std::shared_ptr<Texture> _texture;

	public:
		BlinnPhongMaterial() = default;
	};
}