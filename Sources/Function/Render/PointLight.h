#pragma once

#include"../../Core/Math.h"
#include"Geometry/Sphere.h"
#include"Materials/GeometryMaterial.h"

namespace FOCUS
{
	class PointLight : public Sphere
	{
	public:

		float _intensity{ 1.0f };

		PointLight()
		{
			_radius = 0.1f;
			_color = Vector3(1.0f, 1.0f, 1.0f);
			_intensity = 1.0f;
			_material = std::make_shared<GeometryMaterial>();
		}
	};
}