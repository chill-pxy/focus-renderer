#pragma once

#include<functional>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include<glm/glm.hpp>
#include<glm/gtx/hash.hpp>

namespace FOCUS
{
	using namespace glm;

	typedef glm::mat4 Matrix4;
	typedef glm::mat3 Matrix3;

	typedef glm::vec3 Vector3;
	typedef glm::vec2 Vector2;

	typedef glm::quat Quaternion;

	static const double PI = 3.14159265358979323846264338327950288;


	static float shininessToRoughness(float Ypoint)
	{
		float a = -1;
		float b = 2;

		float c;
		c = (Ypoint / 100) - 1;

		float D;
		D = b * b - (4 * a * c);

		float x1;
		x1 = (-b + sqrt(D)) / (2 * a);

		return x1;
	}
}