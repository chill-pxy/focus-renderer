#include"Math.h"

namespace focus
{
	float shininessToRoughness(float Ypoint)
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

	double halton(int index, int base)
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
}