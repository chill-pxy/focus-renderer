#pragma once

#include"../../Core/Math.h"
#include"RenderResource.h"

namespace FOCUS
{
	class PointLight : public RenderResource
	{
	public:
		Vector3 _position{};

		PointLight() = default;

		virtual void build(std::shared_ptr<DRHI::DynamicRHI> rhi);
		virtual void draw(uint32_t index, std::shared_ptr<DRHI::DynamicRHI> rhi);
	};
}