#pragma once

#include<functional>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include<glm/glm.hpp>
#include<glm/gtx/hash.hpp>

namespace focus
{
	using namespace glm;

	typedef glm::mat4 Matrix4;
	typedef glm::mat3 Matrix3;

	typedef glm::vec3 Vector3;
	typedef glm::vec2 Vector2;

	typedef glm::quat Quaternion;

	static const double PI = 3.14159265358979323846264338327950288;


	float shininessToRoughness(float Ypoint);
	// functions of halton sequence
	double halton(int index, int base);

	std::vector<std::pair<double, double>> generateHaltonSequence(int count);
}