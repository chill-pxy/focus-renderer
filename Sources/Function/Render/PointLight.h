#pragma once

#include"../../Core/Math.h"
#include"Geometry/Sphere.h"
#include"Materials/GeometryMaterial.h"

namespace focus
{
	class PointLight : public Sphere
	{
	public:

		float _intensity{ 1.0f };

		PointLight()
		{
			_radius = 1.0f;
			_color = Vector3(1.0f, 1.0f, 1.0f);
			_intensity = 1.0f;
			_material = std::make_shared<GeometryMaterial>();
			_castShadow = false;
			_isLightActor = true;
		}
	};
}