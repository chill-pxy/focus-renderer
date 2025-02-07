#include<chrono>

#define IMGUI_IMPL_VULKAN_USE_VOLK
#include<imgui.h>
#include<imgui_impl_vulkan.h>
#include<imgui_impl_win32.h>


#include"Renderer.h"
#include"RenderResource.h"

namespace FOCUS
{
	Renderer::Renderer(DRHI::API api, DRHI::PlatformInfo platformCI)
	{
		switch (api)
		{
		default:
		case DRHI::VULKAN:
			DRHI::RHICreateInfo rhiCI{};
			rhiCI.platformInfo = platformCI;
			_rhiContext = std::make_shared<DRHI::VulkanDRHI>(rhiCI);
			break;
		}
	}

	void Renderer::initialize()
	{
		_rhiContext->initialize();

		_rhiContext->createCommandPool(&_shadowCommandPool);
		_rhiContext->createCommandBuffers(&_shadowCommandBuffers, &_shadowCommandPool);

		// initialize shadow map
		// create Depth image
		auto api = _rhiContext->getCurrentAPI();
		auto format = DRHI::DynamicFormat(api);
		auto tilling = DRHI::DynamicImageTiling(api);
		auto useFlag = DRHI::DynamicImageUsageFlagBits(api);
		auto memoryFlag = DRHI::DynamicMemoryPropertyFlags(api);
		auto aspect = DRHI::DynamicImageAspectFlagBits(api);
		auto sampleCount = DRHI::DynamicSampleCountFlags(api);

		_rhiContext->createImage(&_shadowImage, _shadowDepthImageWidth, _shadowDepthImageHeight, format.FORMAT_D16_UNORM, tilling.IMAGE_TILING_OPTIMAL, useFlag.IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | useFlag.IMAGE_USAGE_SAMPLED_BIT, sampleCount.SAMPLE_COUNT_1_BIT , memoryFlag.MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &_shadowImageMemory);
		_rhiContext->createImageView(&_shadowImageView, &_shadowImage, format.FORMAT_D16_UNORM, aspect.IMAGE_ASPECT_DEPTH_BIT);

		// create sampler
		auto borderColor = DRHI::DynamicBorderColor(api);
		auto addressMode = DRHI::DynamicSamplerAddressMode(api);
		DRHI::DynamicSamplerCreateInfo sci{};
		sci.borderColor = borderColor.BORDER_COLOR_FLOAT_OPAQUE_WHITE;
		sci.sampleraAddressMode = addressMode.SAMPLER_ADDRESS_MODE_REPEAT;
		_rhiContext->createSampler(&_shadowSampler, sci);

		_prepared = true;
	}

	void Renderer::buildAndSubmit(std::vector<std::shared_ptr<RenderResource>>* renderlist, std::vector<DRHI::DynamicCommandBuffer>* commandBuffers, DRHI::DynamicCommandPool* commandPool)
	{
		_submitRenderlist = *renderlist;
		_commandBuffers = *commandBuffers;
		_commandPool = *commandPool;

		for (auto p : _submitRenderlist)
		{
			p->build(_rhiContext, &_commandPool, _shadowImage, _shadowImageView, _shadowSampler);
		}

		buildCommandBuffer();
	}

	void Renderer::submitRenderTargetImage(std::vector<DRHI::DynamicImage>* viewportImages, std::vector<DRHI::DynamicImageView>* viewportImageViews, DRHI::DynamicImage* depthImage, DRHI::DynamicImageView* depthImageView)
	{
		_viewportImages = viewportImages;
		_viewportImageViews = viewportImageViews;
		_viewportDepthImage = depthImage;
		_viewportDepthImageView = depthImageView;
	}

	void Renderer::buildCommandBuffer()
	{
		if (_prepared)
		{
			shadowPass();
			scenePass();
		}
	}

	void Renderer::clean()
	{
		_prepared = false;

		_rhiContext->clearImage(&_shadowImageView, &_shadowImage, &_shadowImageMemory);
		_rhiContext->clearSampler(&_shadowSampler);

		_rhiContext->freeCommandBuffers(&_shadowCommandBuffers, &_shadowCommandPool);
		_rhiContext->destroyCommandPool(&_shadowCommandPool);

		_rhiContext->clean();
	}

	void Renderer::recreate()
	{
		_prepared = true;
		buildCommandBuffer();
	}







	//-------------------------- private function ----------------------------
	void Renderer::shadowPass()
	{
		auto aspectFlag = DRHI::DynamicImageAspectFlagBits(_rhiContext->getCurrentAPI());
		auto imageLayout = DRHI::DynamicImageLayout(_rhiContext->getCurrentAPI());

		DRHI::DynamicRenderingInfo renderInfo{};
		renderInfo.isRenderOnSwapChain = false;
		renderInfo.isClearEveryFrame = true;
		renderInfo.includeStencil = false;

		for (int index = 0; index < _shadowCommandBuffers.size(); ++index)
		{
			renderInfo.targetDepthImage = &_shadowImage;
			renderInfo.targetDepthImageView = &_shadowImageView;
			renderInfo.depthAspectFlag = aspectFlag.IMAGE_ASPECT_DEPTH_BIT;
			renderInfo.isClearEveryFrame = true;
			renderInfo.depthImageLayout = imageLayout.IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL;
			renderInfo.targetImageWidth = _shadowDepthImageWidth;
			renderInfo.targetImageHeight = _shadowDepthImageHeight;

			_rhiContext->beginCommandBuffer(_shadowCommandBuffers[index]);
			_rhiContext->beginRendering(_shadowCommandBuffers[index], renderInfo);

			for (auto p : _submitRenderlist)
			{
				if (p->_castShadow)
				{
					p->draw(_rhiContext, &_shadowCommandBuffers[index], true);
				}
			}

			_rhiContext->endRendering(_shadowCommandBuffers[index], renderInfo);
			_rhiContext->endCommandBuffer(_shadowCommandBuffers[index]);
		}
	}

	void Renderer::scenePass()
	{
		auto aspectFlag = DRHI::DynamicImageAspectFlagBits(_rhiContext->getCurrentAPI());
		auto imageLayout = DRHI::DynamicImageLayout(_rhiContext->getCurrentAPI());

		DRHI::DynamicRenderingInfo renderInfo{};
		renderInfo.isRenderOnSwapChain = false;
		renderInfo.isClearEveryFrame = true;
		renderInfo.includeStencil = false;

		for (int index = 0; index < _commandBuffers.size(); ++index)
		{
			renderInfo.targetImage = &(*_viewportImages)[index];
			renderInfo.targetImageView = &(*_viewportImageViews)[index];
			renderInfo.colorAspectFlag = aspectFlag.IMAGE_ASPECT_COLOR_BIT;
			renderInfo.targetDepthImage = _viewportDepthImage;
			renderInfo.targetDepthImageView = _viewportDepthImageView;
			renderInfo.depthAspectFlag = aspectFlag.IMAGE_ASPECT_DEPTH_BIT | aspectFlag.IMAGE_ASPECT_STENCIL_BIT;
			renderInfo.includeStencil = true;
			renderInfo.depthImageLayout = imageLayout.IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			renderInfo.targetImageWidth = _rhiContext->getSwapChainExtentWidth();
			renderInfo.targetImageHeight = _rhiContext->getSwapChainExtentHeight();

			_rhiContext->beginCommandBuffer(_commandBuffers[index]);
			_rhiContext->beginRendering(_commandBuffers[index], renderInfo);

			for (auto p : _submitRenderlist)
			{
				p->draw(_rhiContext, &_commandBuffers[index], false);
			}

			_rhiContext->endRendering(_commandBuffers[index], renderInfo);
			_rhiContext->endCommandBuffer(_commandBuffers[index]);
		}
	}
}