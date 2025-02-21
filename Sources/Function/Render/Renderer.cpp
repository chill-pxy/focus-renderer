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
		// if ray tracing mode is on, renderdoc would throw abort() error
		bool rayTracingMode = false;
		_rhiContext->initialize(rayTracingMode);

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
		sci.sampleraAddressMode = addressMode.SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		_rhiContext->createSampler(&_shadowSampler, sci);

		// prepare environment map
		_rhiContext->createCommandPool(&_environmentMapCommandPool);

		auto texture = loadTexture("../../../Asset/Images/pureSky.hdr");
		_environmentMap = std::make_shared<SkySphere>();
		_environmentMap->initialize(_rhiContext, texture);
		_environmentMap->build(_rhiContext, &_environmentMapCommandPool, _shadowImage, _shadowImageView, _shadowSampler);

		_prepared = true;

		// precomputing
		std::cout << "####################################################" << std::endl;
		precomputeBRDFLUT();
		std::cout << "####################################################" << std::endl;
		precomputeIrradianceMap();
		std::cout << "####################################################" << std::endl;
		prefilterEnvironmentMap();
		std::cout << "####################################################" << std::endl;

		// init ray tracing
		//_rhiContext->initRayTracing();
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

		// shadows
		_rhiContext->clearImage(&_shadowImageView, &_shadowImage, &_shadowImageMemory);
		_rhiContext->clearSampler(&_shadowSampler);

		_rhiContext->freeCommandBuffers(&_shadowCommandBuffers, &_shadowCommandPool);
		_rhiContext->destroyCommandPool(&_shadowCommandPool);

		// environment
		_environmentMap->clean(_rhiContext);
		_rhiContext->destroyCommandPool(&_environmentMapCommandPool);

		// brdf
		_rhiContext->clearImage(&_brdflutImageView, &_brdflutImage, &_brdflutImageMemory);
		_rhiContext->clearSampler(&_brdflutSampler);

		// irradiance
		_rhiContext->clearImage(&_irradianceImageView, &_irradianceImage, &_irradianceImageMemory);
		_rhiContext->clearSampler(&_irradianceSampler);

		// prefilted
		_rhiContext->clearImage(&_filteredImageView, &_filteredImage, &_filteredImageMemory);
		_rhiContext->clearSampler(&_filteredImageSampler);

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

			// draw environment
			_environmentMap->draw(_rhiContext, &_commandBuffers[index], false);

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
		auto tStart = std::chrono::high_resolution_clock::now();

		auto api = _rhiContext->getCurrentAPI();
		auto format = DRHI::DynamicFormat(api);
		auto tilling = DRHI::DynamicImageTiling(api);
		auto usage = DRHI::DynamicImageUsageFlagBits(api);
		auto samples = DRHI::DynamicSampleCountFlags(api);
		auto memory = DRHI::DynamicMemoryPropertyFlagBits(api);
		auto aspect = DRHI::DynamicImageAspectFlagBits(api);
		auto bordercolor = DRHI::DynamicBorderColor(api);
		auto addressmode = DRHI::DynamicSamplerAddressMode(api);

		uint32_t texSize = 512;

		// create brdf lut image
		{
			_rhiContext->createImage(&_brdflutImage, texSize, texSize, format.FORMAT_B8G8R8A8_SRGB, tilling.IMAGE_TILING_OPTIMAL, usage.IMAGE_USAGE_COLOR_ATTACHMENT_BIT | usage.IMAGE_USAGE_SAMPLED_BIT, samples.SAMPLE_COUNT_1_BIT, memory.MEMORY_PROPERTY_DEVICE_LOCAL_BIT,&_brdflutImageMemory);

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
		fcreateInfo.width = texSize;
		fcreateInfo.height = texSize;
		fcreateInfo.layers = 1;

		DRHI::DynamicFramebuffer framebuffer{};
		_rhiContext->createFramebuffer(&framebuffer, &fcreateInfo);

		// descriptors
		DRHI::DynamicDescriptorSetLayout dsl{nullptr};
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
		plci.pushConstantRangeCount = 0;
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
		pipelineci.renderPass = &renderPass;

		DRHI::DynamicPipeline pipeline;
		_rhiContext->createPipeline(&pipeline, &pipelineLayout, pipelineci);

		// start render
		DRHI::DynamicRenderPassBeginInfo beginInfo{};
		beginInfo.framebuffer = framebuffer;
		beginInfo.renderPass = renderPass;
		beginInfo.renderArea.extent.width = texSize;
		beginInfo.renderArea.extent.height = texSize;

		DRHI::DynamicCommandBuffer commandBuffer{};
		DRHI::DynamicCommandPool commandPool{};
		_rhiContext->createCommandPool(&commandPool);
		_rhiContext->createCommandBuffer(&commandBuffer, &commandPool);
	
		_rhiContext->beginCommandBuffer(commandBuffer);

		auto content = DRHI::DynamicSubpassContents(api);
		_rhiContext->beginRenderPass(&commandBuffer, &beginInfo, content.SUBPASS_CONTENTS_INLINE);
		
		DRHI::DynamicViewport viewport{};
		viewport.width = texSize;
		viewport.height = texSize;
		viewport.maxDepth = 1.0f;
		viewport.minDepth = 0.0f;
		_rhiContext->cmdSetViewport(commandBuffer, 0, 1, viewport);

		DRHI::DynamicRect2D scissor{};
		scissor.extent.width = texSize;
		scissor.extent.height = texSize;
		scissor.offset.x = 0;
		scissor.offset.y = 0;
		_rhiContext->cmdSetScissor(commandBuffer, 0, 1, scissor);

		_rhiContext->bindPipeline(pipeline, &commandBuffer, bindPoint.PIPELINE_BIND_POINT_GRAPHICS);
		_rhiContext->cmdDraw(commandBuffer, 3, 1, 0, 0);
		_rhiContext->endRenderPass(&commandBuffer);
		_rhiContext->flushCommandBuffer(commandBuffer, commandPool, true);
		_rhiContext->cmdQueueWaitIdle();

		//free value
		_rhiContext->clearPipeline(&pipeline, &pipelineLayout);
		_rhiContext->clearRenderPass(&renderPass);
		_rhiContext->clearFramebuffer(&framebuffer);
		_rhiContext->clearDescriptorSetLayout(&dsl);
		_rhiContext->clearDescriptorPool(&desciptorPool);
		_rhiContext->destroyCommandPool(&commandPool);

		// cal time
		auto tEnd = std::chrono::high_resolution_clock::now();
		auto tDiff = std::chrono::duration<double, std::milli>(tEnd - tStart).count();
		std::cout << "                                                    " << std::endl;
		std::cout << "      Generating BRDF LUT took" << tDiff << " ms" << std::endl;
		std::cout << "                                                    " << std::endl;
	}

	void Renderer::precomputeIrradianceMap()
	{
		auto tStart = std::chrono::high_resolution_clock::now();

		uint32_t texSize = 64;

		auto api = _rhiContext->getCurrentAPI();
		auto format = DRHI::DynamicFormat(api);
		auto tilling = DRHI::DynamicImageTiling(api);
		auto usage = DRHI::DynamicImageUsageFlagBits(api);
		auto samples = DRHI::DynamicSampleCountFlags(api);
		auto memory = DRHI::DynamicMemoryPropertyFlagBits(api);
		auto aspect = DRHI::DynamicImageAspectFlagBits(api);
		auto bordercolor = DRHI::DynamicBorderColor(api);
		auto addressmode = DRHI::DynamicSamplerAddressMode(api);
		auto imageflags = DRHI::DynamicImageCreateFlags(api);
		auto viewType = DRHI::DynamicImageViewType(api);

		// create irradiance image
		{
			DRHI::DynamicImageCreateInfo imageci{};
			imageci.flags = imageflags.IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
			imageci.format = format.FORMAT_B8G8R8A8_SRGB;
			imageci.extent.width = texSize;
			imageci.extent.height = texSize;
			imageci.extent.depth = 1;
			imageci.mipLevels = 1;
			imageci.arrayLayers = 6;
			imageci.samples = samples.SAMPLE_COUNT_1_BIT;
			imageci.tiling = tilling.IMAGE_TILING_OPTIMAL;
			imageci.usage = usage.IMAGE_USAGE_SAMPLED_BIT | usage.IMAGE_USAGE_TRANSFER_DST_BIT;
			_rhiContext->createImage(&_irradianceImage, &_irradianceImageMemory, imageci, memory.MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

			// create irradiance image view
			DRHI::DynamicImageViewCreateInfo vci{};
			vci.type = viewType.IMAGE_VIEW_TYPE_CUBE;
			vci.format = format.FORMAT_B8G8R8A8_SRGB;
			vci.image = _irradianceImage;
			vci.subresourceRange.aspectMask = aspect.IMAGE_ASPECT_COLOR_BIT;
			vci.subresourceRange.layerCount = 6;
			vci.subresourceRange.levelCount = 1;
			_rhiContext->createImageView(&_irradianceImageView, &_irradianceImage, vci);

			// create irradiance image sampler
			auto mipmap = DRHI::DynamicSamplerMipmapMode(api);
			DRHI::DynamicSamplerCreateInfo sci{};
			sci.borderColor = bordercolor.BORDER_COLOR_FLOAT_OPAQUE_WHITE;
			sci.sampleraAddressMode = addressmode.SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			sci.mipmapMode = mipmap.SAMPLER_MIPMAP_MODE_LINEAR;
			sci.minLod = 0.0f;
			sci.maxLod = 1;
			_rhiContext->createSampler(&_irradianceSampler, sci);
		}

		// attachment
		auto loadOp = DRHI::DynamicAttachmentLoadOp(api);
		auto storeOp = DRHI::DynamicAttachmentStoreOp(api);
		auto layout = DRHI::DynamicImageLayout(api);
		DRHI::DynamicAttachmentDescription ad{};
		ad.format = format.FORMAT_B8G8R8A8_SRGB;
		ad.samples = samples.SAMPLE_COUNT_1_BIT;
		ad.loadOp = loadOp.ATTACHMENT_LOAD_OP_CLEAR;
		ad.storeOp = storeOp.ATTACHMENT_STORE_OP_STORE;
		ad.stencilLoadOp = loadOp.ATTACHMENT_LOAD_OP_DONT_CARE;
		ad.stencilStoreOp = storeOp.ATTACHMENT_STORE_OP_DONT_CARE;
		ad.initialLayout = layout.IMAGE_LAYOUT_UNDEFINED;
		ad.finalLayout = layout.IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		DRHI::DynamicAttachmentReference ar{};
		ar.attachment = 0;
		ar.layout = layout.IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		// subpass description
		auto bindPoint = DRHI::DynamicPipelineBindPoint(api);
		DRHI::DynamicSubpassDescription subpassDescription{};
		subpassDescription.pipelineBindPoint = bindPoint.PIPELINE_BIND_POINT_GRAPHICS;
		subpassDescription.colorAttachmentCount = 1;
		subpassDescription.pColorAttachments = &ar;

		// subpass dependency
		auto stage = DRHI::DynamicPipelineStageFlags(api);
		auto access = DRHI::DynamicAccessFlagBits(api);
		auto dependcyFlag = DRHI::DynamicDependencyFlagBits(api);
		std::vector<DRHI::DynamicSubpassDependency> dependencies{ 2 };
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

		// create offscreen image
		DRHI::DynamicImage        irradianceOffscreenImage{};
		DRHI::DynamicDeviceMemory irradianceOffscreenImageMemory{};
		DRHI::DynamicImageView    irradianceOffscreenImageView{};
		DRHI::DynamicSampler      irradianceOffscreenSampler{};

		DRHI::DynamicImageCreateInfo ici{};
		ici.format = format.FORMAT_B8G8R8A8_SRGB;
		ici.extent.width = texSize;
		ici.extent.height = texSize;
		ici.extent.depth = 1;
		ici.mipLevels = 1;
		ici.arrayLayers = 1;
		ici.samples = samples.SAMPLE_COUNT_1_BIT;
		ici.tiling = tilling.IMAGE_TILING_OPTIMAL;
		ici.initialLayout = layout.IMAGE_LAYOUT_UNDEFINED;
		ici.usage = usage.IMAGE_USAGE_COLOR_ATTACHMENT_BIT | usage.IMAGE_USAGE_TRANSFER_SRC_BIT;
		_rhiContext->createImage(&irradianceOffscreenImage, &irradianceOffscreenImageMemory, ici, memory.MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		DRHI::DynamicImageViewCreateInfo ivci{};
		ivci.format = format.FORMAT_B8G8R8A8_SRGB;
		ivci.image = irradianceOffscreenImage;
		ivci.type = viewType.IMAGE_VIEW_TYPE_2D;
		ivci.subresourceRange.aspectMask = aspect.IMAGE_ASPECT_COLOR_BIT;
		ivci.subresourceRange.baseMipLevel = 0;
		ivci.subresourceRange.levelCount = 1;
		ivci.subresourceRange.baseArrayLayer = 0;
		ivci.subresourceRange.layerCount = 1;

		_rhiContext->createImageView(&irradianceOffscreenImageView, &irradianceOffscreenImage, ivci);

		// create framebuffer
		DRHI::DynamicFramebufferCreateInfo fcreateInfo{};
		fcreateInfo.renderPass = renderPass;
		fcreateInfo.attachmentCount = 1;
		fcreateInfo.pAttachments = &irradianceOffscreenImageView;
		fcreateInfo.width = texSize;
		fcreateInfo.height = texSize;
		fcreateInfo.layers = 1;

		DRHI::DynamicFramebuffer framebuffer{};
		_rhiContext->createFramebuffer(&framebuffer, &fcreateInfo);

		// set image layout
		DRHI::DynamicCommandPool commandPool{};
		_rhiContext->createCommandPool(&commandPool);

		// once commandbuffer
		DRHI::DynamicCommandBuffer onceCmdBuf{};
		_rhiContext->createCommandBuffer(&onceCmdBuf, &commandPool);

		_rhiContext->beginCommandBuffer(onceCmdBuf);

		_rhiContext->setImageLayout(&onceCmdBuf, &irradianceOffscreenImage, aspect.IMAGE_ASPECT_COLOR_BIT,
			layout.IMAGE_LAYOUT_UNDEFINED, layout.IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
		_rhiContext->flushCommandBuffer(onceCmdBuf, commandPool, true);

		// descriptors
		auto descriptorType = DRHI::DynamicDescriptorType(api);
		auto stageFlags = DRHI::DynamicShaderStageFlags(api);
		DRHI::DynamicDescriptorSetLayout dsl{ nullptr };
		std::vector<DRHI::DynamicDescriptorSetLayoutBinding> dsbs(1);
		dsbs[0].binding = 0;
		dsbs[0].descriptorCount = 1;
		dsbs[0].descriptorType = descriptorType.DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		dsbs[0].pImmutableSamplers = nullptr;
		dsbs[0].stageFlags = stageFlags.SHADER_STAGE_FRAGMENT_BIT;
		_rhiContext->createDescriptorSetLayout(&dsl, &dsbs);

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

		DRHI::DynamicDescriptorImageInfo dii{};
		dii.imageLayout = layout.IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		dii.imageView = _environmentMap->_material->_textureImageView;
		dii.sampler = _environmentMap->_material->_textureSampler;

		std::vector<DRHI::DynamicWriteDescriptorSet> wds(1);
		wds[0].descriptorType = desciptorType.DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		wds[0].dstBinding = 0;
		wds[0].descriptorCount = 1;
		wds[0].pImageInfo = &dii;

		_rhiContext->createDescriptorSet(&desciptorSet, &dsl, &desciptorPool, &wds, 1);

		// pipeline layout
		struct PushBlock
		{
			Matrix4 mvp;
			float deltaPhi = (2.0f * float(PI) / 180.f);
			float deltaTheta = (0.5f * float(PI) / 64.f);
		}pushBlock;

		auto pushStage = DRHI::DynamicShaderStageFlags(api);
		DRHI::DynamicPushConstantRange pcr{};
		pcr.stageFlags = pushStage.SHADER_STAGE_VERTEX_BIT | pushStage.SHADER_STAGE_FRAGMENT_BIT;
		pcr.offset = 0;
		pcr.size = sizeof(PushBlock);

		DRHI::DynamicPipelineLayout pipelineLayout{};
		DRHI::DynamicPipelineLayoutCreateInfo plci{};
		plci.pPushConstantRanges = &pcr;
		plci.pushConstantRangeCount = 1;
		plci.pSetLayouts = &dsl;
		plci.setLayoutCount = 1;
		_rhiContext->createPipelineLayout(&pipelineLayout, &plci);

		// pipeline
		auto cullmode = DRHI::DynamicCullMode(api);
		DRHI::DynamicPipelineCreateInfo pipelineci{};
		pipelineci.vertexShader = "../../../Shaders/IBL/prefilterCubeVertex.spv";
		pipelineci.fragmentShader = "../../../Shaders/IBL/irradianceMapFragment.spv";
		pipelineci.vertexInputBinding = DRHI::DynamicVertexInputBindingDescription();
		pipelineci.vertexInputBinding.set(api, 0, sizeof(Vertex));
		pipelineci.vertexInputAttributes = std::vector<DRHI::DynamicVertexInputAttributeDescription>();
		pipelineci.vertexInputAttributes.resize(1);
		pipelineci.vertexInputAttributes[0].set(api, 0, 0, format.FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Vertex::pos));
		pipelineci.colorImageFormat = format.FORMAT_B8G8R8A8_SRGB;
		pipelineci.depthImageFormat = format.FORMAT_UNDEFINED;
		pipelineci.includeStencil = false;
		pipelineci.dynamicDepthBias = false;
		pipelineci.cullMode = cullmode.CULL_MODE_BACK_BIT;
		pipelineci.sampleCounts = samples.SAMPLE_COUNT_1_BIT;
		pipelineci.renderPass = &renderPass;

		DRHI::DynamicPipeline pipeline;
		_rhiContext->createPipeline(&pipeline, &pipelineLayout, pipelineci);

		// rendering
		auto content = DRHI::DynamicSubpassContents(api);
		DRHI::DynamicRenderPassBeginInfo binfo{};
		binfo.framebuffer = framebuffer;
		binfo.renderPass = renderPass;
		binfo.renderArea.extent.width = texSize;
		binfo.renderArea.extent.height = texSize;
		binfo.renderArea.offset.x = 0;
		binfo.renderArea.offset.y = 0;

		// prepare command buffer
		DRHI::DynamicCommandBuffer commandBuffer{};
		_rhiContext->createCommandBuffer(&commandBuffer, &commandPool);

		_rhiContext->beginCommandBuffer(commandBuffer);

		// scissor
		DRHI::DynamicViewport viewport{};
		viewport.width = texSize;
		viewport.height = texSize;
		viewport.maxDepth = 1.0f;
		viewport.minDepth = 0.0f;
		_rhiContext->cmdSetViewport(commandBuffer, 0, 1, viewport);

		DRHI::DynamicRect2D scissor{};
		scissor.extent.width = texSize;
		scissor.extent.height = texSize;
		scissor.offset.x = 0;
		scissor.offset.y = 0;
		_rhiContext->cmdSetScissor(commandBuffer, 0, 1, scissor);

		// rotation matrix
		std::vector<Matrix4> matrices = {
			rotate(rotate(Matrix4(1.0f), radians(90.0f), Vector3(0.0f, 1.0f, 0.0f)), radians(180.0f), Vector3(1.0f, 0.0f, 0.0f)),
			rotate(rotate(Matrix4(1.0f), radians(-90.0f), Vector3(0.0f, 1.0f, 0.0f)), radians(180.0f), Vector3(1.0f, 0.0f, 0.0f)),
			rotate(Matrix4(1.0f), radians(-90.0f), Vector3(1.0f, 0.0f, 0.0f)),
			rotate(Matrix4(1.0f), radians(90.0f), Vector3(1.0f, 0.0f, 0.0f)),
			rotate(Matrix4(1.0f), radians(180.0f), Vector3(1.0f, 0.0f, 0.0f)),
			rotate(Matrix4(1.0f), radians(180.0f), Vector3(0.0f, 0.0f, 1.0f)),
		};

		// set image layout
		DRHI::DynamicImageSubresourceRange range{};
		range.aspectMask = aspect.IMAGE_ASPECT_COLOR_BIT;
		range.baseMipLevel = 0;
		range.levelCount = 1;
		range.layerCount = 6;
		_rhiContext->setImageLayout(&commandBuffer, &_irradianceImage, layout.IMAGE_LAYOUT_UNDEFINED, layout.IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, range);

		// start render pass
		for (uint32_t f = 0; f < 6; ++f)
		{
			viewport.width = static_cast<float>(texSize);
			viewport.height = static_cast<float>(texSize);
			_rhiContext->cmdSetViewport(commandBuffer, 0, 1, viewport);

			_rhiContext->beginRenderPass(&commandBuffer, &binfo, content.SUBPASS_CONTENTS_INLINE);

			pushBlock.mvp = perspective((float)(PI / 2.0), 1.0f, 0.1f, 512.0f) * matrices[f];

			_rhiContext->cmdPushConstants(&pipelineLayout, &commandBuffer, pushStage.SHADER_STAGE_VERTEX_BIT | pushStage.SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushBlock), &pushBlock);

			_environmentMap->draw(_rhiContext, &commandBuffer, pipeline, pipelineLayout, desciptorSet);

			_rhiContext->endRenderPass(&commandBuffer);

			_rhiContext->setImageLayout(&commandBuffer, &irradianceOffscreenImage, aspect.IMAGE_ASPECT_COLOR_BIT, layout.IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, layout.IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
		
			DRHI::DynamicImageCopy copy{};
			copy.srcSubresource.aspectMask = aspect.IMAGE_ASPECT_COLOR_BIT;
			copy.srcSubresource.baseArrayLayer = 0;
			copy.srcSubresource.mipLevel = 0;
			copy.srcSubresource.layerCount = 1;
			copy.srcOffset = { 0,0,0 };

			copy.dstSubresource.aspectMask = aspect.IMAGE_ASPECT_COLOR_BIT;
			copy.dstSubresource.baseArrayLayer = f;
			copy.dstSubresource.mipLevel = 0;
			copy.dstSubresource.layerCount = 1;
			copy.dstOffset = { 0,0,0 };

			copy.extent = { texSize, texSize, 1 };
			
			_rhiContext->cmdCopyImage(commandBuffer, &irradianceOffscreenImage, layout.IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, &_irradianceImage, layout.IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, copy);
		
			_rhiContext->setImageLayout(&commandBuffer, &irradianceOffscreenImage, aspect.IMAGE_ASPECT_COLOR_BIT, layout.IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, layout.IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
		}

		_rhiContext->setImageLayout(&commandBuffer, &_irradianceImage, layout.IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, layout.IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, range);

		_rhiContext->flushCommandBuffer(commandBuffer, commandPool, true);

		_rhiContext->clearRenderPass(&renderPass);
		_rhiContext->clearFramebuffer(&framebuffer);
		_rhiContext->clearImage(&irradianceOffscreenImageView, &irradianceOffscreenImage, &irradianceOffscreenImageMemory);
		_rhiContext->clearSampler(&irradianceOffscreenSampler);
		_rhiContext->clearDescriptorPool(&desciptorPool);
		_rhiContext->clearDescriptorSetLayout(&dsl);
		_rhiContext->clearPipeline(&pipeline, &pipelineLayout);
		_rhiContext->destroyCommandPool(&commandPool);

		// cal time
		auto tEnd = std::chrono::high_resolution_clock::now();
		auto tDiff = std::chrono::duration<double, std::milli>(tEnd - tStart).count();
		std::cout << "                                                    " << std::endl;
		std::cout << "    Generating Irradiance Map took" << tDiff << " ms" << std::endl;
		std::cout << "                                                    " << std::endl;
	}

	void Renderer::prefilterEnvironmentMap()
	{
		auto tStart = std::chrono::high_resolution_clock::now();

		uint32_t texSize = 512;

		auto api = _rhiContext->getCurrentAPI();
		auto format = DRHI::DynamicFormat(api);
		auto tilling = DRHI::DynamicImageTiling(api);
		auto usage = DRHI::DynamicImageUsageFlagBits(api);
		auto samples = DRHI::DynamicSampleCountFlags(api);
		auto memory = DRHI::DynamicMemoryPropertyFlagBits(api);
		auto aspect = DRHI::DynamicImageAspectFlagBits(api);
		auto bordercolor = DRHI::DynamicBorderColor(api);
		auto addressmode = DRHI::DynamicSamplerAddressMode(api);
		auto imageflags = DRHI::DynamicImageCreateFlags(api);
		auto viewType = DRHI::DynamicImageViewType(api);

		// create image
		{
			DRHI::DynamicImageCreateInfo imageCI{};
			imageCI.format = format.FORMAT_B8G8R8A8_SRGB;
			imageCI.extent.width = texSize;
			imageCI.extent.height = texSize;
			imageCI.extent.depth = 1;
			imageCI.mipLevels = 1;
			imageCI.arrayLayers = 6;
			imageCI.samples = samples.SAMPLE_COUNT_1_BIT;
			imageCI.tiling = tilling.IMAGE_TILING_OPTIMAL;
			imageCI.usage = usage.IMAGE_USAGE_SAMPLED_BIT | usage.IMAGE_USAGE_TRANSFER_DST_BIT;
			imageCI.flags = imageflags.IMAGE_CREATE_CUBE_COMPATIBLE_BIT;

			_rhiContext->createImage(&_filteredImage, &_filteredImageMemory, imageCI, memory.MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		
			DRHI::DynamicImageViewCreateInfo viewInfo{};
			viewInfo.format = format.FORMAT_B8G8R8A8_SRGB;
			viewInfo.image = _filteredImage;
			viewInfo.type = viewType.IMAGE_VIEW_TYPE_CUBE;
			viewInfo.subresourceRange.aspectMask = aspect.IMAGE_ASPECT_COLOR_BIT;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.layerCount = 6;
			viewInfo.subresourceRange.levelCount = 1;

			_rhiContext->createImageView(&_filteredImageView, &_filteredImage, viewInfo);

			auto mipmap = DRHI::DynamicSamplerMipmapMode(api);
			DRHI::DynamicSamplerCreateInfo samplerInfo{};
			samplerInfo.borderColor = bordercolor.BORDER_COLOR_FLOAT_OPAQUE_WHITE;
			samplerInfo.maxLod = 1;
			samplerInfo.minLod = 0.0f;
			samplerInfo.mipmapMode = mipmap.SAMPLER_MIPMAP_MODE_LINEAR;
			samplerInfo.sampleraAddressMode = addressmode.SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

			_rhiContext->createSampler(&_filteredImageSampler, samplerInfo);
		}

		// color attachment
		auto loadOp = DRHI::DynamicAttachmentLoadOp(api);
		auto storeOp = DRHI::DynamicAttachmentStoreOp(api);
		auto layout = DRHI::DynamicImageLayout(api);
		DRHI::DynamicAttachmentDescription ad{};
		ad.format = format.FORMAT_B8G8R8A8_SRGB;
		ad.samples = samples.SAMPLE_COUNT_1_BIT;
		ad.loadOp = loadOp.ATTACHMENT_LOAD_OP_CLEAR;
		ad.storeOp = storeOp.ATTACHMENT_STORE_OP_STORE;
		ad.stencilLoadOp = loadOp.ATTACHMENT_LOAD_OP_DONT_CARE;
		ad.stencilStoreOp = storeOp.ATTACHMENT_STORE_OP_DONT_CARE;
		ad.initialLayout = layout.IMAGE_LAYOUT_UNDEFINED;
		ad.finalLayout = layout.IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		DRHI::DynamicAttachmentReference ar{};
		ar.attachment = 0;
		ar.layout = layout.IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		
		// subpass description
		auto bindPoint = DRHI::DynamicPipelineBindPoint(api);
		DRHI::DynamicSubpassDescription subpassDescription{};
		subpassDescription.pipelineBindPoint = bindPoint.PIPELINE_BIND_POINT_GRAPHICS;
		subpassDescription.colorAttachmentCount = 1;
		subpassDescription.pColorAttachments = &ar;

		// subpass dependencies
		auto stage = DRHI::DynamicPipelineStageFlags(api);
		auto access = DRHI::DynamicAccessFlagBits(api);
		auto dependcyFlag = DRHI::DynamicDependencyFlagBits(api);
		std::vector<DRHI::DynamicSubpassDependency> dependencies{ 2 };
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

		// create renderpass
		DRHI::DynamicRenderPassCreateInfo renderPassCI{};
		renderPassCI.attachmentCount = 1;
		renderPassCI.pAttachments = &ad;
		renderPassCI.subpassCount = 1;
		renderPassCI.pSubpasses = &subpassDescription;
		renderPassCI.dependencyCount = 2;
		renderPassCI.pDependencies = &dependencies;

		DRHI::DynamicRenderPass renderPass{};
		_rhiContext->createRenderPass(&renderPass, &renderPassCI);

		// create offscreen image
		DRHI::DynamicImage        offscreenImage{};
		DRHI::DynamicDeviceMemory offscreenImageMemory{};
		DRHI::DynamicImageView    offscreenImageView{};
		DRHI::DynamicSampler      offscreenSampler{};

		DRHI::DynamicImageCreateInfo ici{};
		ici.format = format.FORMAT_B8G8R8A8_SRGB;
		ici.extent.width = texSize;
		ici.extent.height = texSize;
		ici.extent.depth = 1;
		ici.mipLevels = 1;
		ici.arrayLayers = 1;
		ici.samples = samples.SAMPLE_COUNT_1_BIT;
		ici.tiling = tilling.IMAGE_TILING_OPTIMAL;
		ici.initialLayout = layout.IMAGE_LAYOUT_UNDEFINED;
		ici.usage = usage.IMAGE_USAGE_COLOR_ATTACHMENT_BIT | usage.IMAGE_USAGE_TRANSFER_SRC_BIT;
		_rhiContext->createImage(&offscreenImage, &offscreenImageMemory, ici, memory.MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		DRHI::DynamicImageViewCreateInfo ivci{};
		ivci.format = format.FORMAT_B8G8R8A8_SRGB;
		ivci.image = offscreenImage;
		ivci.type = viewType.IMAGE_VIEW_TYPE_2D;
		ivci.subresourceRange.aspectMask = aspect.IMAGE_ASPECT_COLOR_BIT;
		ivci.subresourceRange.baseMipLevel = 0;
		ivci.subresourceRange.levelCount = 1;
		ivci.subresourceRange.baseArrayLayer = 0;
		ivci.subresourceRange.layerCount = 1;

		_rhiContext->createImageView(&offscreenImageView, &offscreenImage, ivci);

		// create framebuffer
		DRHI::DynamicFramebufferCreateInfo fcreateInfo{};
		fcreateInfo.renderPass = renderPass;
		fcreateInfo.attachmentCount = 1;
		fcreateInfo.pAttachments = &offscreenImageView;
		fcreateInfo.width = texSize;
		fcreateInfo.height = texSize;
		fcreateInfo.layers = 1;

		DRHI::DynamicFramebuffer framebuffer{};
		_rhiContext->createFramebuffer(&framebuffer, &fcreateInfo);

		DRHI::DynamicCommandPool cmdPool{};
		DRHI::DynamicCommandBuffer cmdBuffer{};
		_rhiContext->createCommandPool(&cmdPool);
		_rhiContext->createCommandBuffer(&cmdBuffer, &cmdPool);
		_rhiContext->beginCommandBuffer(cmdBuffer);
		_rhiContext->setImageLayout(&cmdBuffer, &offscreenImage,aspect.IMAGE_ASPECT_COLOR_BIT, layout.IMAGE_LAYOUT_UNDEFINED, layout.IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
		_rhiContext->flushCommandBuffer(cmdBuffer, cmdPool, true);

		// descriptors
		auto descriptorType = DRHI::DynamicDescriptorType(api);
		auto stageFlags = DRHI::DynamicShaderStageFlags(api);
		DRHI::DynamicDescriptorSetLayout dsl{ nullptr };
		std::vector<DRHI::DynamicDescriptorSetLayoutBinding> dsbs(1);
		dsbs[0].binding = 0;
		dsbs[0].descriptorCount = 1;
		dsbs[0].descriptorType = descriptorType.DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		dsbs[0].pImmutableSamplers = nullptr;
		dsbs[0].stageFlags = stageFlags.SHADER_STAGE_FRAGMENT_BIT;
		_rhiContext->createDescriptorSetLayout(&dsl, &dsbs);

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

		DRHI::DynamicDescriptorImageInfo dii{};
		dii.imageLayout = layout.IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		dii.imageView = _environmentMap->_material->_textureImageView;
		dii.sampler = _environmentMap->_material->_textureSampler;

		std::vector<DRHI::DynamicWriteDescriptorSet> wds(1);
		wds[0].descriptorType = desciptorType.DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		wds[0].dstBinding = 0;
		wds[0].descriptorCount = 1;
		wds[0].pImageInfo = &dii;

		_rhiContext->createDescriptorSet(&desciptorSet, &dsl, &desciptorPool, &wds, 1);

		// pipeline layout
		struct PushBlock {
			Matrix4 mvp;
			float roughness;
			uint32_t numSamples = 32u;
		} pushBlock;

		auto pushStage = DRHI::DynamicShaderStageFlags(api);
		DRHI::DynamicPushConstantRange pcr{};
		pcr.stageFlags = pushStage.SHADER_STAGE_VERTEX_BIT | pushStage.SHADER_STAGE_FRAGMENT_BIT;
		pcr.offset = 0;
		pcr.size = sizeof(PushBlock);

		DRHI::DynamicPipelineLayout pipelineLayout{};
		DRHI::DynamicPipelineLayoutCreateInfo plci{};
		plci.pPushConstantRanges = &pcr;
		plci.pushConstantRangeCount = 1;
		plci.pSetLayouts = &dsl;
		plci.setLayoutCount = 1;
		_rhiContext->createPipelineLayout(&pipelineLayout, &plci);

		// pipeline
		auto cullmode = DRHI::DynamicCullMode(api);
		DRHI::DynamicPipelineCreateInfo pipelineci{};
		pipelineci.vertexShader = "../../../Shaders/IBL/prefilterCubeVertex.spv";
		pipelineci.fragmentShader = "../../../Shaders/IBL/prefilterEnvmapFragment.spv";
		pipelineci.vertexInputBinding = DRHI::DynamicVertexInputBindingDescription();
		pipelineci.vertexInputBinding.set(api, 0, sizeof(Vertex));
		pipelineci.vertexInputAttributes = std::vector<DRHI::DynamicVertexInputAttributeDescription>();
		pipelineci.vertexInputAttributes.resize(1);
		pipelineci.vertexInputAttributes[0].set(api, 0, 0, format.FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Vertex::pos));
		pipelineci.colorImageFormat = format.FORMAT_B8G8R8A8_SRGB;
		pipelineci.depthImageFormat = format.FORMAT_UNDEFINED;
		pipelineci.includeStencil = false;
		pipelineci.dynamicDepthBias = false;
		pipelineci.cullMode = cullmode.CULL_MODE_BACK_BIT;
		pipelineci.sampleCounts = samples.SAMPLE_COUNT_1_BIT;
		pipelineci.renderPass = &renderPass;

		DRHI::DynamicPipeline pipeline{};
		_rhiContext->createPipeline(&pipeline, &pipelineLayout, pipelineci);

		// begin render pass
		DRHI::DynamicRenderPassBeginInfo rbinfo{};
		rbinfo.renderPass = renderPass;
		rbinfo.framebuffer = framebuffer;
		rbinfo.renderArea.extent.width = texSize;
		rbinfo.renderArea.extent.height = texSize;
		
		std::vector<Matrix4> matrices = {
			// POSITIVE_X
			rotate(rotate(Matrix4(1.0f), radians(90.0f), Vector3(0.0f, 1.0f, 0.0f)), radians(180.0f), Vector3(1.0f, 0.0f, 0.0f)),
			// NEGATIVE_X
			rotate(rotate(Matrix4(1.0f), radians(-90.0f), Vector3(0.0f, 1.0f, 0.0f)), radians(180.0f), Vector3(1.0f, 0.0f, 0.0f)),
			// POSITIVE_Y
			rotate(Matrix4(1.0f), radians(-90.0f), Vector3(1.0f, 0.0f, 0.0f)),
			// NEGATIVE_Y
			rotate(Matrix4(1.0f), radians(90.0f), Vector3(1.0f, 0.0f, 0.0f)),
			// POSITIVE_Z
			rotate(Matrix4(1.0f), radians(180.0f), Vector3(1.0f, 0.0f, 0.0f)),
			// NEGATIVE_Z
			rotate(Matrix4(1.0f), radians(180.0f), Vector3(0.0f, 0.0f, 1.0f)),
		};

		DRHI::DynamicCommandBuffer commandBuffer{};
		_rhiContext->createCommandBuffer(&commandBuffer, &cmdPool);
		_rhiContext->beginCommandBuffer(commandBuffer);

		DRHI::DynamicViewport viewPort{};
		viewPort.width = texSize;
		viewPort.height = texSize;
		viewPort.minDepth = 0.0f;
		viewPort.maxDepth = 1.0f;
		viewPort.x = 0;
		viewPort.y = 0;

		_rhiContext->cmdSetViewport(commandBuffer, 0, 1, viewPort);

		DRHI::DynamicRect2D scissor{};
		scissor.extent.width = texSize;
		scissor.extent.height = texSize;
		scissor.offset.x = 0;
		scissor.offset.y = 0;
		_rhiContext->cmdSetScissor(commandBuffer, 0, 1, scissor);

		DRHI::DynamicImageSubresourceRange range{};
		range.aspectMask = aspect.IMAGE_ASPECT_COLOR_BIT;
		range.baseMipLevel = 0;
		range.levelCount = 1;
		range.layerCount = 6;
		_rhiContext->setImageLayout(&commandBuffer, &_filteredImage, layout.IMAGE_LAYOUT_UNDEFINED, layout.IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, range);

		auto content = DRHI::DynamicSubpassContents(api);
		auto shaderStage = DRHI::DynamicShaderStageFlags(api);
		// rendering
		for (uint32_t f = 0; f < 6; ++f)
		{
			_rhiContext->beginRenderPass(&commandBuffer, &rbinfo, content.SUBPASS_CONTENTS_INLINE);

			pushBlock.mvp = perspective((float)(PI / 2.0), 1.0f, 0.1f, 512.f) * matrices[f];

			_rhiContext->cmdPushConstants(&pipelineLayout, &commandBuffer, shaderStage.SHADER_STAGE_VERTEX_BIT | shaderStage.SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushBlock), &pushBlock);

			_rhiContext->bindPipeline(pipeline, &commandBuffer, bindPoint.PIPELINE_BIND_POINT_GRAPHICS);
			_rhiContext->bindDescriptorSets(&desciptorSet, pipelineLayout, &commandBuffer, bindPoint.PIPELINE_BIND_POINT_GRAPHICS);

			_environmentMap->draw(_rhiContext, &commandBuffer, pipeline, pipelineLayout, desciptorSet);

			_rhiContext->endRenderPass(&commandBuffer);

			_rhiContext->setImageLayout(&commandBuffer, &offscreenImage, aspect.IMAGE_ASPECT_COLOR_BIT, layout.IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, layout.IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
	
			DRHI::DynamicImageCopy copy{};
			copy.srcSubresource.aspectMask = aspect.IMAGE_ASPECT_COLOR_BIT;
			copy.srcSubresource.baseArrayLayer = 0;
			copy.srcSubresource.mipLevel = 0;
			copy.srcSubresource.layerCount = 1;
			copy.srcOffset = { 0,0,0 };

			copy.dstSubresource.aspectMask = aspect.IMAGE_ASPECT_COLOR_BIT;
			copy.dstSubresource.baseArrayLayer = f;
			copy.dstSubresource.mipLevel = 0;
			copy.dstSubresource.layerCount = 1;
			copy.dstOffset = { 0,0,0 };

			copy.extent = { texSize, texSize, 1 };

			_rhiContext->cmdCopyImage(commandBuffer, &offscreenImage, layout.IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, &_filteredImage, layout.IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, copy);

			_rhiContext->setImageLayout(&commandBuffer, &offscreenImage, aspect.IMAGE_ASPECT_COLOR_BIT, layout.IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, layout.IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);	
		}

		_rhiContext->setImageLayout(&commandBuffer, &_filteredImage, layout.IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, layout.IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, range);

		_rhiContext->flushCommandBuffer(commandBuffer, cmdPool, true);

		_rhiContext->clearRenderPass(&renderPass);
		_rhiContext->clearFramebuffer(&framebuffer);

		_rhiContext->clearImage(&offscreenImageView, &offscreenImage, &offscreenImageMemory);
		_rhiContext->clearSampler(&offscreenSampler);

		_rhiContext->clearDescriptorPool(&desciptorPool);
		_rhiContext->clearDescriptorSetLayout(&dsl);

		_rhiContext->clearPipeline(&pipeline, &pipelineLayout);

		_rhiContext->destroyCommandPool(&cmdPool);

		// cal time
		auto tEnd = std::chrono::high_resolution_clock::now();
		auto tDiff = std::chrono::duration<double, std::milli>(tEnd - tStart).count();
		std::cout << "                                                    " << std::endl;
		std::cout << "    Prefilter Environment Map took" << tDiff << " ms" << std::endl;
		std::cout << "                                                    " << std::endl;
	}
}