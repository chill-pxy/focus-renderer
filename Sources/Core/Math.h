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

	// functions of halton sequence
	//{
		static double halton(int index, int base)
		{
			double f = 1.0;
			double r = 0.0;
			while (index > 0)
			{
				f /= base;
				r += f * (index % base);
				index /= base;
			}

			return r;
		}

		std::vector<std::pair<double, double>> generateHaltonSequence(int count)
		{
			std::vector<std::pair<double, double>> sequence(count);

			int baseX = 2;
			int baseY = 3;

			for (int i = 0; i < count; ++i)
			{
				sequence[i].first = halton(i + 1, baseX);
				sequence[i].second = halton(i + 1, baseY);
			}

			return sequence;
		}
	//}
}