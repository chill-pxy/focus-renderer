#pragma once

#include"Geometry/Sphere.h"
#include"Materials/EnvironmentMap.h"

namespace FOCUS
{
	class SkySphere : public Sphere
	{
	public:
		SkySphere()
		{
			
		}

		void initialize(std::shared_ptr<DRHI::DynamicRHI> rhi ,std::shared_ptr<Texture> texture)
		{
			_material = std::make_shared<EnvironmentMap>(texture);
			_material->_cullMode = DRHI::DynamicCullMode(rhi->getCurrentAPI()).CULL_MODE_FRONT_BIT;
			_scale = Vector3(1000, 1000, 1000);
			_rotation = rotate(identity<Matrix4>(), radians(90.0f), Vector3(-1, 0, 0));
			_castShadow = false;
		}
	};
}