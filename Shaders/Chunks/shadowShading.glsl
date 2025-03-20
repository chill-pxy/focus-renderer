#define LIGHT_WORLD_SIZE 0.05
#define LIGHT_FRUSTUM_WIDTH 6.0
#define LIGHT_SIZE_UV (LIGHT_WORLD_SIZE / LIGHT_FRUSTUM_WIDTH)
#define PI 3.1415926535897932384626433832795
#define PI2 6.283185307179586
#define SAMPLES 50

float unpack(vec4 rgbaDepth) 
{
	const vec4 bitShift = vec4(1.0, 1.0 / 256.0, 1.0 / (256.0 * 256.0), 1.0 / (256.0 * 256.0 * 256.0));
	return dot(rgbaDepth, bitShift);
}

// shadow mapping
float shadowMapping(sampler2D shadowMap, vec4 shadowCoord)
{
	float depth = unpack(texture(shadowMap, shadowCoord.xy));
	float cur_depth = shadowCoord.z;
	if (cur_depth >= depth + 1e-3 )
	{
		return 0.0;
	}
	else 
	{
		return 1.0;
	}
}

// pcf
float PCF(sampler2D shadowMap, vec4 shadowCoord, float scale)
{
	ivec2 texDim = textureSize(shadowMap, 0);

	float dx = scale * 0.1 / float(texDim.x);
	float dy = scale * 0.1 / float(texDim.y);

	float shadowFactor = 0.0;
	int count = 0;
	int range = 3;

	for (int x = -range; x <= range; x++)
	{
		for (int y = -range; y <= range; y++)
		{
			shadowFactor += shadowMapping(shadowMap, vec4(shadowCoord.x + (x * dx), shadowCoord.y + (y*dy), shadowCoord.z, shadowCoord.w));// , vec2(dx * x, dy * y));
			count++;
		}
	}
	return shadowFactor / count;
}

float rand_2to1(vec2 uv) 
{
	// 0 - 1
	const float a = 12.9898, b = 78.233, c = 43758.5453;
	float dt = dot(uv.xy, vec2(a, b)), sn = mod(dt, PI);
	return fract(sin(sn) * c);
}

vec2 poissonDisk[SAMPLES];
void poissonDiskSamples(const in vec2 randomSeed)
{
	int rings = 10;
	float ANGLE_STEP = PI2 * float(rings) / float(SAMPLES);
	float INV_NUM_SAMPLES = 1.0 / float(SAMPLES);

	float angle = rand_2to1(randomSeed) * PI2;
	float radius = INV_NUM_SAMPLES;
	float radiusStep = radius;

	for (int i = 0; i < SAMPLES; i++) {
		poissonDisk[i] = vec2(cos(angle), sin(angle)) * pow(radius, 0.75);
		radius += radiusStep;
		angle += ANGLE_STEP;
	}
}

float PCFrandomSamples(sampler2D shadowMap, vec4 shadowCoord, float scale)
{
	poissonDiskSamples(shadowCoord.xy);

	float visibility = 0.0;

	for (int i = 0; i < SAMPLES; i++)
	{
		vec2 offset = poissonDisk[i] / 1500;
		float shadowDepth = shadowMapping(shadowMap, shadowCoord + vec4(offset, 0.0, 0.0));
		if (shadowCoord.z > shadowDepth + 1e-3)
		{
			visibility++;
		}
	}

	return 1.0 - visibility / float(SAMPLES);
}

// pcss
float PCSS(sampler2D shadowMap, vec4 shadowCoord)
{
	float zRecevier = shadowCoord.z;

	float avgBlockDepth = 1.0;

	// calculate blocker averager depth
	int blockerNum = 0;
	float blockDepth = 0.0;

	float searchRadius = LIGHT_SIZE_UV * (shadowCoord.z - 0.01) / shadowCoord.z;

	for (int i = 0; i < 20; i++)
	{
		float shadowDepth = unpack(texture(shadowMap, shadowCoord.xy * searchRadius));
		if (zRecevier > shadowDepth)
		{
			blockerNum++;
			blockDepth += shadowDepth;
		}
	}

	if (blockerNum == 0)
		avgBlockDepth = -1.0;
	else
		avgBlockDepth = blockDepth / float(blockerNum);

	float penumbra = (zRecevier - avgBlockDepth) * LIGHT_SIZE_UV / avgBlockDepth;

	return PCF(shadowMap, shadowCoord, penumbra);
}