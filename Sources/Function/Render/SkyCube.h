#pragma once

#include"Materials/EnvironmentCube.h"
#include"Geometry/Box.h"

namespace FOCUS
{
	class SkyCube : public Box
	{
	public:

		SkyCube()
		{

		}

		void initialize(std::shared_ptr<DRHI::DynamicRHI> rhi, std::shared_ptr<Texture> texture)
		{
			_material = std::make_shared<EnvironmentCube>(texture);
			_material->_cullMode = DRHI::DynamicCullMode(rhi->getCurrentAPI()).CULL_MODE_FRONT_BIT;
			_scale = Vector3(10, 10, 10);
			_castShadow = false;
		}

		void clean(std::shared_ptr<DRHI::DynamicRHI> rhi)
		{
			__super::clean(rhi);
			_shadow->clean(rhi);
			_material->clean(rhi);
		}
	};
}