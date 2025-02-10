#include<chrono>
#include<array>

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

		////for test
		precomputeBRDFLUT();
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

	void Renderer::precomputeBRDFLUT()
	{
		auto api = _rhiContext->getCurrentAPI();
		auto format = DRHI::DynamicFormat(api);
		auto tilling = DRHI::DynamicImageTiling(api);
		auto usage = DRHI::DynamicImageUsageFlagBits(api);
		auto samples = DRHI::DynamicSampleCountFlags(api);
		auto memory = DRHI::DynamicMemoryPropertyFlagBits(api);
		auto aspect = DRHI::DynamicImageAspectFlagBits(api);
		auto bordercolor = DRHI::DynamicBorderColor(api);
		auto addressmode = DRHI::DynamicSamplerAddressMode(api);

		// create brdf lut image
		{
			_rhiContext->createImage(&_brdflutImage, 512, 512, format.FORMAT_B8G8R8A8_SRGB, tilling.IMAGE_TILING_OPTIMAL, usage.IMAGE_USAGE_COLOR_ATTACHMENT_BIT | usage.IMAGE_USAGE_SAMPLED_BIT, samples.SAMPLE_COUNT_1_BIT, memory.MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &_brdflutImageMemory);

			// create brdf lut image view
			_rhiContext->createImageView(&_brdflutImageView, &_brdflutImage, format.FORMAT_B8G8R8A8_SRGB, aspect.IMAGE_ASPECT_COLOR_BIT);

			// create brdf lut image sampler
			DRHI::DynamicSamplerCreateInfo sci{};
			sci.borderColor = bordercolor.BORDER_COLOR_FLOAT_OPAQUE_WHITE;
			sci.sampleraAddressMode = addressmode.SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			_rhiContext->createSampler(&_brdflutSampler, sci);
		}

		// prepare resources for creating render pass
		auto loadOp = DRHI::DynamicAttachmentLoadOp(api);
		auto storeOp = DRHI::DynamicAttachmentStoreOp(api);
		auto layout = DRHI::DynamicImageLayout(api);

		// prepare for attachment
		DRHI::DynamicAttachmentDescription ad{};
		ad.format = format.FORMAT_B8G8R8A8_SRGB;
		ad.samples = samples.SAMPLE_COUNT_1_BIT;
		ad.loadOp = loadOp.ATTACHMENT_LOAD_OP_CLEAR;
		ad.storeOp = storeOp.ATTACHMENT_STORE_OP_STORE;
		ad.stencilLoadOp = loadOp.ATTACHMENT_LOAD_OP_DONT_CARE;
		ad.stencilStoreOp = storeOp.ATTACHMENT_STORE_OP_DONT_CARE;
		ad.initialLayout = layout.IMAGE_LAYOUT_UNDEFINED;
		ad.finalLayout = layout.IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		DRHI::DynamicAttachmentReference colorRef{};
		colorRef.attachment = 0;
		colorRef.layout = layout.IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		auto bindPoint = DRHI::DynamicPipelineBindPoint(api);
		DRHI::DynamicSubpassDescription subpassDescription{};
		subpassDescription.pipelineBindPoint = bindPoint.PIPELINE_BIND_POINT_GRAPHICS;
		subpassDescription.colorAttachmentCount = 1;
		subpassDescription.pColorAttachments = &colorRef;

		auto stage = DRHI::DynamicPipelineStageFlags(api);
		auto access = DRHI::DynamicAccessFlagBits(api);
		auto dependcyFlag = DRHI::DynamicDependencyFlagBits(api);
		std::vector<DRHI::DynamicSubpassDependency> dependencies{2};
		dependencies[0].srcSubpass = ~0U;
		dependencies[0].dstSubpass = 0;
		dependencies[0].srcStageMask = stage.PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		dependencies[0].dstStageMask = stage.PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependencies[0].srcAccessMask = access.ACCESS_MEMORY_READ_BIT;
		dependencies[0].dstAccessMask = access.ACCESS_COLOR_ATTACHMENT_READ_BIT | access.ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependencies[0].dependencyFlags = dependcyFlag.DEPENDENCY_BY_REGION_BIT;
		
		dependencies[1].srcSubpass = 0;
		dependencies[1].dstSubpass = ~0U;
		dependencies[1].srcStageMask = stage.PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependencies[1].dstStageMask = stage.PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		dependencies[1].srcAccessMask = access.ACCESS_COLOR_ATTACHMENT_READ_BIT | access.ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependencies[1].dstAccessMask = access.ACCESS_MEMORY_READ_BIT;
		dependencies[1].dependencyFlags = dependcyFlag.DEPENDENCY_BY_REGION_BIT;

		// create render pass
		DRHI::DynamicRenderPassCreateInfo rpcreateInfo{};
		rpcreateInfo.attachmentCount = 1;
		rpcreateInfo.pAttachments = &ad;
		rpcreateInfo.subpassCount = 1;
		rpcreateInfo.pSubpasses = &subpassDescription;
		rpcreateInfo.dependencyCount = 2;
		rpcreateInfo.pDependencies = &dependencies;

		DRHI::DynamicRenderPass renderPass{};
		_rhiContext->createRenderPass(&renderPass, &rpcreateInfo);

		// create framebuffer
		DRHI::DynamicFramebufferCreateInfo fcreateInfo{};
		fcreateInfo.renderPass = renderPass;
		fcreateInfo.attachmentCount = 1;
		fcreateInfo.pAttachments = &_brdflutImageView;
		fcreateInfo.width = 512;
		fcreateInfo.height = 512;
		fcreateInfo.layers = 1;

		DRHI::DynamicFramebuffer framebuffer{};
		_rhiContext->createFramebuffer(&framebuffer, &fcreateInfo);

		// descriptors
		DRHI::DynamicDescriptorSetLayout dsl{};
		std::vector<DRHI::DynamicDescriptorSetLayoutBinding> bindings{};
		_rhiContext->createDescriptorSetLayout(&dsl, &bindings);

		// descriptor pool
		auto desciptorType = DRHI::DynamicDescriptorType(api);
		std::vector<DRHI::DynamicDescriptorPoolSize> poolSizes{ 1 };
		poolSizes[0].descriptorCount = 1;
		poolSizes[0].type = desciptorType.DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

		DRHI::DynamicDescriptorPoolCreateInfo pci{};
		pci.pPoolSizes = &poolSizes;
		pci.maxSets = 2;

		DRHI::DynamicDescriptorPool desciptorPool;
		_rhiContext->createDescriptorPool(&desciptorPool, &pci);

		// descriptor sets
		DRHI::DynamicDescriptorSet desciptorSet;
		_rhiContext->createDescriptorSet(&desciptorSet, &dsl, &desciptorPool, nullptr, 1);

		// pipeline layout
		DRHI::DynamicPipelineLayout pipelineLayout{};
		DRHI::DynamicPipelineLayoutCreateInfo plci{};
		plci.pPushConstantRanges = 0;
		plci.pSetLayouts = &dsl;
		plci.setLayoutCount = 1;
		_rhiContext->createPipelineLayout(&pipelineLayout, &plci);

		// pipeline
		auto cullmode = DRHI::DynamicCullMode(api);
		DRHI::DynamicPipelineCreateInfo pipelineci{};
		pipelineci.vertexShader = "../../../Shaders/IBL/brdflutVertex.spv";
		pipelineci.fragmentShader = "../../../Shaders/IBL/brdflutFragment.spv";
		pipelineci.vertexInputBinding = DRHI::DynamicVertexInputBindingDescription();
		pipelineci.vertexInputBinding.set(api, 0, sizeof(Vertex));
		pipelineci.vertexInputAttributes = std::vector<DRHI::DynamicVertexInputAttributeDescription>();
		pipelineci.colorImageFormat = format.FORMAT_B8G8R8A8_SRGB;
		pipelineci.depthImageFormat = format.FORMAT_UNDEFINED;
		pipelineci.includeStencil = false;
		pipelineci.dynamicDepthBias = false;
		pipelineci.cullMode = cullmode.CULL_MODE_BACK_BIT;
		pipelineci.sampleCounts = samples.SAMPLE_COUNT_1_BIT;

		DRHI::DynamicPipeline pipeline;
		_rhiContext->createPipeline(&pipeline, &pipelineLayout, pipelineci);
}

	void Renderer::prefilterEnvironmentMap()
	{

	}
}