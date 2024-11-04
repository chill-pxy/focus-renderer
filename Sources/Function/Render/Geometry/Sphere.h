#pragma once

#include"MeshVertex.h"
#include"../RenderResource.h"

namespace FOCUS
{
	class Sphere : public RenderResource
	{
	public:
		virtual void build(std::shared_ptr<DRHI::DynamicRHI> rhi)
		{

		}

		virtual void draw(uint32_t index, std::shared_ptr<DRHI::DynamicRHI> rhi)
		{

		}
	};
}