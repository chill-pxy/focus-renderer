#pragma once

#include"Geometry/Sphere.h"
#include"Materials/EnvironmentMap.h"

namespace focus
{
	class SkySphere : public Sphere
	{
	public:
		SkySphere()
		{
			
		}

		void initialize(std::shared_ptr<drhi::DynamicRHI> rhi ,std::shared_ptr<Texture> texture)
		{
			_material = std::make_shared<EnvironmentMap>(texture);
			_material->_cullMode = drhi::DynamicCullMode(rhi->getCurrentAPI()).CULL_MODE_FRONT_BIT;
			_scale = Vector3(1000, 1000, 1000);
			_rotation = rotate(identity<Matrix4>(), radians(90.0f), Vector3(-1, 0, 0));
			_castShadow = false;
		}

		void clean(std::shared_ptr<drhi::DynamicRHI> rhi)
		{
			__super::clean(rhi);
			_shadow->clean(rhi);
			_material->clean(rhi);
			_deffered->clean(rhi);
		}
	};
}