vec2 reprojection(vec2 uv, Sampler2D depthBuffer, mat4 currentInvViewProj, mat4 preViewProj)
{
	float depth = texture(depthBuffer, uv).r;

	vec4 clipPos = vec4(uv.x * 2 - 1, (1 - uv.y) * 2 - 1, depth, 1.0);

	mat4 worldPos = mul(currentInvViewProj, clipPos);
	worldPos /= worldPos.w;

	vec4 preClipPos = mul(preViewProj, worldPos);
}