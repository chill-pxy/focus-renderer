#include"ShadowMap.h"

namespace FOCUS
{
	void ShadowMap::initialize()
	{
		auto api = _rhi->getCurrentAPI();
		auto format = DRHI::DynamicFormat(api);
		auto tilling = DRHI::DynamicImageTiling(api);
		auto useFlag = DRHI::DynamicImageUsageFlagBits(api);
		auto memoryFlag = DRHI::DynamicMemoryPropertyFlags(api);
		
		// create Depth image
		_rhi->createImage(&_depthImage, _shadowDepthImageWidth, _shadowDepthImageHeight,
			format.FORMAT_D16_UNORM, tilling.IMAGE_TILING_OPTIMAL, useFlag.IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | useFlag.IMAGE_USAGE_SAMPLED_BIT,
			memoryFlag.MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &_depthImageMemory);

		// create Depth image view
		auto imageAspect = DRHI::DynamicImageAspectFlagBits(api);
		_rhi->createImageView(&_depthImageView, &_depthImage, format.FORMAT_D16_UNORM, imageAspect.IMAGE_ASPECT_DEPTH_BIT);
	
		// create sampler
		auto borderColor = DRHI::DynamicBorderColor(api);
		auto addressMode = DRHI::DynamicSamplerAddressMode(api);
		DRHI::DynamicSamplerCreateInfo sci{};
		sci.borderColor = borderColor.BORDER_COLOR_FLOAT_OPAQUE_WHITE;
		sci.sampleraAddressMode = addressMode.SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		_rhi->createSampler(&_shadowSampler, sci);
	}
}