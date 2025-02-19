#define LIGHT_WORLD_SIZE 0.05
#define LIGHT_FRUSTUM_WIDTH 6.0
#define LIGHT_SIZE_UV (LIGHT_WORLD_SIZE / LIGHT_FRUSTUM_WIDTH)

float unpack(vec4 rgbaDepth) 
{
	const vec4 bitShift = vec4(1.0, 1.0 / 256.0, 1.0 / (256.0 * 256.0), 1.0 / (256.0 * 256.0 * 256.0));
	return dot(rgbaDepth, bitShift);
}

// shadow mapping
float shadowMapping(sampler2D shadowMap, vec4 shadowCoord, vec2 off)
{
	float shadow = 1.0;
	if (shadowCoord.z > -1.0 && shadowCoord.z < 1.0)
	{
		float dist = texture(shadowMap, shadowCoord.st + off).r;
		if (shadowCoord.w > 0.0 && dist < shadowCoord.z)
		{
			shadow = 0.1;
		}
	}
	return shadow;
}

// pcf
float PCF(sampler2D shadowMap, vec4 shadowCoord, float scale)
{
	ivec2 texDim = textureSize(shadowMap, 0);

	float dx = scale * 1.0 / float(texDim.x);
	float dy = scale * 1.0 / float(texDim.y);

	float shadowFactor = 0.0;
	int count = 0;
	int range = 3;

	for (int x = -range; x <= range; x++)
	{
		for (int y = -range; y <= range; y++)
		{
			shadowFactor += shadowMapping(shadowMap, shadowCoord, vec2(dx * x, dy * y));
			count++;
		}

	}
	return shadowFactor / count;
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