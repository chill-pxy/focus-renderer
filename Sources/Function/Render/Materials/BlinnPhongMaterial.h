#pragma once

#include<memory>

#include<drhi.h>

#include"Material.h"
#include"../Texture.h"

namespace FOCUS
{
	class BlinnPhongMaterial : public Material
	{
	public:
		std::shared_ptr<Texture> _texture;
		const char* _vertexShader = "../../../Shaders/blinnPhongVertex.spv";
		const char* _fragmentShader = "../../../Shaders/blinnPhongFragment.spv";

	public:
		BlinnPhongMaterial() = default;
		
		virtual void build(std::shared_ptr<DRHI::DynamicRHI> rhi) {};
		virtual void updateUniformBuffer(uint32_t currentImage, Matrix4 view) {};
	};
}