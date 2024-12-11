#pragma once

#include<drhi.h>

namespace FOCUS
{
	class ShadowMap
	{
	public:
		std::shared_ptr<DRHI::DynamicRHI> _rhi;

		DRHI::DynamicPipeline _shadowPipeline{};
		DRHI::DynamicPipelineLayout _shadowPipelineLayout{};

		DRHI::DynamicDescriptorSet _shadowDescriptorSet{};

		DRHI::DynamicImage _depthImage{};
		DRHI::DynamicDeviceMemory _depthImageMemory{};
		DRHI::DynamicImageView _depthImageView{};
		DRHI::DynamicSampler _shadowSampler{};

		uint32_t _shadowDepthImageWidth{ 512 };
		uint32_t _shadowDepthImageHeight{ 512 };

	public:
		ShadowMap() = default;

		void initialize();
	};
}