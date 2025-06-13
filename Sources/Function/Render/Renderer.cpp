#include<chrono>
#include<array>

// imgui api
#define IMGUI_IMPL_VULKAN_USE_VOLK
#include<imgui.h>
#include<imgui_impl_vulkan.h>
#include<imgui_impl_win32.h>

#include"Renderer.h"
#include"RenderResource.h"

namespace focus
{
	Renderer::Renderer(drhi::API api, drhi::PlatformInfo platformCI)
	{
		switch (api)
		{
		default:
		case drhi::VULKAN:
			drhi::RHICreateInfo rhiCI{};
			rhiCI.platformInfo = platformCI;
			_rhiContext = std::make_shared<drhi::VulkanDRHI>(rhiCI);
			break;
		}
	}

	void Renderer::initialize()
	{
		// if ray tracing mode is on, renderdoc would throw abort() error
		bool rayTracingMode = false;
		_rhiContext->initialize(rayTracingMode);

		{
			// initialize scene command
			_rhiContext->createCommandPool(&_sceneCommandPool);
			_rhiContext->createCommandBuffers(&_sceneCommandBuffers, &_sceneCommandPool);
			for (auto& cmdbuf : _sceneCommandBuffers)
			{
				cmdbuf._name = "scene command buffer";
			}

			// initialize shadow map
			_rhiContext->createCommandPool(&_shadowCommandPool);
			_rhiContext->createCommandBuffers(&_shadowCommandBuffers, &_shadowCommandPool);
			for (auto& cmdbuf : _shadowCommandBuffers)
			{
				cmdbuf._name = "shadow command buffer";
			}

			auto api = _rhiContext->getCurrentAPI();
			auto format = drhi::DynamicFormat(api);
			auto tilling = drhi::DynamicImageTiling(api);
			auto useFlag = drhi::DynamicImageUsageFlagBits(api);
			auto memoryFlag = drhi::DynamicMemoryPropertyFlags(api);
			auto aspect = drhi::DynamicImageAspectFlagBits(api);
			auto sampleCount = drhi::DynamicSampleCountFlags(api);

			_rhiContext->createImage(&_shadowImage, _shadowDepthImageWidth, _shadowDepthImageHeight, format.FORMAT_D16_UNORM, tilling.IMAGE_TILING_OPTIMAL, useFlag.IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | useFlag.IMAGE_USAGE_SAMPLED_BIT, sampleCount.SAMPLE_COUNT_1_BIT, memoryFlag.MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &_shadowImageMemory);
			_rhiContext->createImageView(&_shadowImageView, &_shadowImage, format.FORMAT_D16_UNORM, aspect.IMAGE_ASPECT_DEPTH_BIT);

			auto borderColor = drhi::DynamicBorderColor(api);
			auto addressMode = drhi::DynamicSamplerAddressMode(api);
			drhi::DynamicSamplerCreateInfo sci{};
			sci.borderColor = borderColor.BORDER_COLOR_FLOAT_OPAQUE_WHITE;
			sci.sampleraAddressMode = addressMode.SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			_rhiContext->createSampler(&_shadowSampler, sci);

			// initialize deffered
			_rhiContext->createCommandPool(&_defferedCommandPool);
			_rhiContext->createCommandBuffers(&_defferedCommandBuffer, &_defferedCommandPool);

			auto bordercolor = drhi::DynamicBorderColor(api);
			auto addressmode = drhi::DynamicSamplerAddressMode(api);
			auto mipmap = drhi::DynamicSamplerMipmapMode(api);
			drhi::DynamicSamplerCreateInfo samplerInfo{};

			// position
			{
				_rhiContext->createImage(&_position, _rhiContext->getSwapChainExtentWidth(), _rhiContext->getSwapChainExtentHeight(), format.FORMAT_R16G16B16A16_SFLOAT, tilling.IMAGE_TILING_OPTIMAL, useFlag.IMAGE_USAGE_COLOR_ATTACHMENT_BIT | useFlag.IMAGE_USAGE_SAMPLED_BIT, sampleCount.SAMPLE_COUNT_1_BIT, memoryFlag.MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &_positionMemory);
				_rhiContext->createImageView(&_positionView, &_position, format.FORMAT_R16G16B16A16_SFLOAT, aspect.IMAGE_ASPECT_COLOR_BIT);

				samplerInfo.borderColor = bordercolor.BORDER_COLOR_FLOAT_OPAQUE_WHITE;
				samplerInfo.maxLod = 1;
				samplerInfo.minLod = 0.0f;
				samplerInfo.mipmapMode = mipmap.SAMPLER_MIPMAP_MODE_LINEAR;
				samplerInfo.sampleraAddressMode = addressmode.SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

				_rhiContext->createSampler(&_positionSampler, samplerInfo);
			}

			// depth
			_rhiContext->createDepthStencil(&_depth, &_depthView, &_depthMemory, format.FORMAT_D32_SFLOAT_S8_UINT, _rhiContext->getSwapChainExtentWidth(), _rhiContext->getSwapChainExtentHeight(), sampleCount.SAMPLE_COUNT_1_BIT);

			// normal
			{
				_rhiContext->createImage(&_normal, _rhiContext->getSwapChainExtentWidth(), _rhiContext->getSwapChainExtentHeight(), format.FORMAT_R16G16B16A16_SFLOAT, tilling.IMAGE_TILING_OPTIMAL, useFlag.IMAGE_USAGE_COLOR_ATTACHMENT_BIT | useFlag.IMAGE_USAGE_SAMPLED_BIT, sampleCount.SAMPLE_COUNT_1_BIT, memoryFlag.MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &_normalMemory);
				_rhiContext->createImageView(&_normalView, &_normal, format.FORMAT_R16G16B16A16_SFLOAT, aspect.IMAGE_ASPECT_COLOR_BIT);

				samplerInfo.borderColor = bordercolor.BORDER_COLOR_FLOAT_OPAQUE_WHITE;
				samplerInfo.maxLod = 1;
				samplerInfo.minLod = 0.0f;
				samplerInfo.mipmapMode = mipmap.SAMPLER_MIPMAP_MODE_LINEAR;
				samplerInfo.sampleraAddressMode = addressmode.SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

				_rhiContext->createSampler(&_normalSampler, samplerInfo);
			}

			// albedo
			{
				_rhiContext->createImage(&_albedo, _rhiContext->getSwapChainExtentWidth(), _rhiContext->getSwapChainExtentHeight(), format.FORMAT_B8G8R8A8_UNORM, tilling.IMAGE_TILING_OPTIMAL, useFlag.IMAGE_USAGE_COLOR_ATTACHMENT_BIT | useFlag.IMAGE_USAGE_SAMPLED_BIT, sampleCount.SAMPLE_COUNT_1_BIT, memoryFlag.MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &_albedoMemory);
				_rhiContext->createImageView(&_albedoView, &_albedo, format.FORMAT_B8G8R8A8_UNORM, aspect.IMAGE_ASPECT_COLOR_BIT);

				samplerInfo.borderColor = bordercolor.BORDER_COLOR_FLOAT_OPAQUE_WHITE;
				samplerInfo.maxLod = 1;
				samplerInfo.minLod = 0.0f;
				samplerInfo.mipmapMode = mipmap.SAMPLER_MIPMAP_MODE_LINEAR;
				samplerInfo.sampleraAddressMode = addressmode.SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

				_rhiContext->createSampler(&_albedoSampler, samplerInfo);
			}

			// initialize motion vector
			//_rhiContext->createImage(&_motionImage, _rhiContext->getSwapChainExtentWidth(), _rhiContext->getSwapChainExtentHeight(), format.FORMAT_B8G8R8A8_UNORM, tilling.IMAGE_TILING_OPTIMAL, useFlag.IMAGE_USAGE_COLOR_ATTACHMENT_BIT | useFlag.IMAGE_USAGE_SAMPLED_BIT, sampleCount.SAMPLE_COUNT_1_BIT, memoryFlag.MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &_normalMemory);
			//_rhiContext->createImageView(&_motionImageView, &_motionImage, format.FORMAT_B8G8R8A8_UNORM, aspect.IMAGE_ASPECT_COLOR_BIT);

			//samplerInfo.borderColor = bordercolor.BORDER_COLOR_FLOAT_OPAQUE_WHITE;
			//samplerInfo.maxLod = 1;
			//samplerInfo.minLod = 0.0f;
			//samplerInfo.mipmapMode = mipmap.SAMPLER_MIPMAP_MODE_LINEAR;
			//samplerInfo.sampleraAddressMode = addressmode.SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

			//_rhiContext->createSampler(&_motionSampler, samplerInfo);
		}

		// prepare environment map
		{
			_rhiContext->createCommandPool(&_environmentMapCommandPool);

			auto texture = loadCubeTexture(RESOURCE_PATH"Asset/Images/hall.ktx");
			_filteredImageWidth = texture->_width;
			_filteredImageHeight = texture->_height;
			_environmentMap = std::make_shared<SkyCube>();
			_environmentMap->initialize(_rhiContext, texture);
			_environmentMap->preBuild(_rhiContext, &_environmentMapCommandPool);
			//_environmentMap->build(_rhiContext, &_environmentMapCommandPool);
		}

		_prepared = true;

		// precomputing
		{
			std::cout << "####################################################" << std::endl;
			precomputeBRDFLUT();
			std::cout << "####################################################" << std::endl;
			precomputeIrradianceCube();
			std::cout << "####################################################" << std::endl;
			prefilterEnvironmentCube();
			std::cout << "####################################################" << std::endl;
		}

		// init ray tracing
		//_rhiContext->initRayTracing();
	}

	void Renderer::buildAndSubmit(std::vector<std::shared_ptr<RenderResource>>* renderlist)
	{
		if (_submitRenderlist != nullptr)
			_submitRenderlist = nullptr;

		_submitRenderlist = renderlist;
		_environmentMap->_material->_defferdDraw = true;
		_environmentMap->_built = false;
		_submitRenderlist->push_back(_environmentMap);

		for (auto p : *_submitRenderlist)
		{
			if (p->_built) continue;
			
			//preapre ibl resources
			{
				p->_material->_brdfImageView = &_brdflutImageView;
				p->_material->_brdfSampler = &_brdflutSampler;

				p->_material->_irradianceImageView = &_irradianceImageView;
				p->_material->_irradianceSampler = &_irradianceSampler;

				p->_material->_filteredImageView = &_filteredImageView;
				p->_material->_filteredImageSampler = &_filteredImageSampler;
			}

			// prepare shadow resources
			{
				p->_material->_shadowImageView = &_shadowImageView;
				p->_material->_shadowSampler = &_shadowSampler;
			}

			// prepare g-buffer resources
			{
				p->_material->_gbuffer.albedoImageView = &_albedoView;
				p->_material->_gbuffer.albedoSampler = &_albedoSampler;
				p->_material->_gbuffer.positionImageView = &_positionView;
				p->_material->_gbuffer.positionSampler = &_positionSampler;
				p->_material->_gbuffer.normalImageView = &_normalView;
				p->_material->_gbuffer.normalSampler = &_normalSampler;
				p->_material->_gbuffer.depthImageView = &_depthView;
				p->_material->_gbuffer.depthSampler = &_depthSampler;
			}

			p->build(_rhiContext, &_sceneCommandPool);
		}

		buildCommandBuffer();
	}

	void Renderer::submitRenderTargetImage(std::vector<drhi::DynamicImage>* viewportImages, std::vector<drhi::DynamicImageView>* viewportImageViews, drhi::DynamicImage* depthImage, drhi::DynamicImageView* depthImageView)
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
			defferedPass();
			shadowPass();
			scenePass();
		}
	}

	void Renderer::clean()
	{
		_prepared = false;

		// scene
		_rhiContext->freeCommandBuffers(&_sceneCommandBuffers, &_sceneCommandPool);
		_rhiContext->destroyCommandPool(&_sceneCommandPool);

		// shadows
		_rhiContext->clearImage(&_shadowImageView, &_shadowImage, &_shadowImageMemory);
		_rhiContext->clearSampler(&_shadowSampler);

		_rhiContext->freeCommandBuffers(&_shadowCommandBuffers, &_shadowCommandPool);
		_rhiContext->destroyCommandPool(&_shadowCommandPool);

		// deffered
		_rhiContext->clearImage(&_positionView, &_position, &_positionMemory);
		_rhiContext->clearSampler(&_positionSampler);

		_rhiContext->clearImage(&_albedoView, &_albedo, &_albedoMemory);
		_rhiContext->clearSampler(&_albedoSampler);

		_rhiContext->clearImage(&_normalView, &_normal, &_normalMemory);
		_rhiContext->clearSampler(&_normalSampler);

		_rhiContext->clearImage(&_depthView, &_depth, &_depthMemory);
		_rhiContext->clearSampler(&_depthSampler);

		_rhiContext->freeCommandBuffers(&_defferedCommandBuffer, & _defferedCommandPool);
		_rhiContext->destroyCommandPool(&_defferedCommandPool);

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
		auto aspectFlag = drhi::DynamicImageAspectFlagBits(_rhiContext->getCurrentAPI());
		auto imageLayout = drhi::DynamicImageLayout(_rhiContext->getCurrentAPI());

		drhi::DynamicRenderingInfo renderInfo{};
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

			for (auto p : *_submitRenderlist)
			{
				if (p->_castShadow)
				{
					p->draw(_rhiContext, &_shadowCommandBuffers[index], RenderResourcePipeline::SHADOW);
				}
			}

			_rhiContext->endRendering(_shadowCommandBuffers[index], renderInfo);
			_rhiContext->endCommandBuffer(_shadowCommandBuffers[index]);
		}
	}

	void Renderer::scenePass()
	{
		auto api = _rhiContext->getCurrentAPI();
		auto aspectFlag = drhi::DynamicImageAspectFlagBits(api);
		auto imageLayout = drhi::DynamicImageLayout(api);
		auto bindPoint = drhi::DynamicPipelineBindPoint(api);

		drhi::DynamicRenderingInfo renderInfo{};
		renderInfo.isRenderOnSwapChain = false;
		renderInfo.isClearEveryFrame = true;
		renderInfo.includeStencil = false;

		auto pipeline = _submitRenderlist->at(1)->_material->_pipeline;
		auto descriptorSet = _submitRenderlist->at(1)->_material->_descriptorSet;
		auto pipelineLayout = _submitRenderlist->at(1)->_material->_pipelineLayout;

		for (int index = 0; index < _sceneCommandBuffers.size(); ++index)
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

			_rhiContext->beginCommandBuffer(_sceneCommandBuffers[index]);
			_rhiContext->beginRendering(_sceneCommandBuffers[index], renderInfo);

			_rhiContext->bindDescriptorSets(&descriptorSet, pipelineLayout, &_sceneCommandBuffers[index], bindPoint.PIPELINE_BIND_POINT_GRAPHICS);
			_rhiContext->bindPipeline(pipeline, &_sceneCommandBuffers[index], bindPoint.PIPELINE_BIND_POINT_GRAPHICS);
			
			_rhiContext->draw(&_sceneCommandBuffers[index], 3, 1, 0, 0);

			_rhiContext->endRendering(_sceneCommandBuffers[index], renderInfo);
			_rhiContext->endCommandBuffer(_sceneCommandBuffers[index]);
		}
	}

	void Renderer::defferedPass()
	{
		auto aspectFlag = drhi::DynamicImageAspectFlagBits(_rhiContext->getCurrentAPI());
		auto imageLayout = drhi::DynamicImageLayout(_rhiContext->getCurrentAPI());

		drhi::DynamicRenderingMRTInfo renderInfo{};
		renderInfo.isClearEveryFrame = true;
		renderInfo.includeStencil = false;
		auto target = std::vector<drhi::DynamicImage>{  _albedo, _position, _normal };
		auto targetView = std::vector<drhi::DynamicImageView>{  _albedoView, _positionView, _normalView };

		for (int index = 0; index < _defferedCommandBuffer.size(); ++index)
		{
			renderInfo.targetImage = &target;
			renderInfo.targetImageView = &targetView;
			renderInfo.colorAspectFlag = aspectFlag.IMAGE_ASPECT_COLOR_BIT;
			renderInfo.targetDepthImage = &_depth;
			renderInfo.targetDepthImageView = &_depthView;
			renderInfo.depthAspectFlag = aspectFlag.IMAGE_ASPECT_DEPTH_BIT | aspectFlag.IMAGE_ASPECT_STENCIL_BIT;
			renderInfo.includeStencil = true;
			renderInfo.depthImageLayout = imageLayout.IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			renderInfo.targetImageWidth = _rhiContext->getSwapChainExtentWidth();
			renderInfo.targetImageHeight = _rhiContext->getSwapChainExtentHeight();

			_rhiContext->beginCommandBuffer(_defferedCommandBuffer[index]);
			_rhiContext->beginRendering(_defferedCommandBuffer[index], &renderInfo);

			// draw environment map
			_environmentMap->draw(_rhiContext, &_defferedCommandBuffer[index], RenderResourcePipeline::DEFFERED);

			// draw scene
			for (auto p : *_submitRenderlist)
			{
				if (p->_isLightActor) continue;
				p->draw(_rhiContext, &_defferedCommandBuffer[index], RenderResourcePipeline::DEFFERED);
			}

			

			_rhiContext->endRendering(_defferedCommandBuffer[index], &renderInfo);
			_rhiContext->endCommandBuffer(_defferedCommandBuffer[index]);
		}
	}

	void Renderer::postEffectPass()
	{

	}

	void Renderer::precomputeBRDFLUT()
	{
		auto tStart = std::chrono::high_resolution_clock::now();

		auto api = _rhiContext->getCurrentAPI();
		auto format = drhi::DynamicFormat(api);
		auto tilling = drhi::DynamicImageTiling(api);
		auto usage = drhi::DynamicImageUsageFlagBits(api);
		auto samples = drhi::DynamicSampleCountFlags(api);
		auto memory = drhi::DynamicMemoryPropertyFlagBits(api);
		auto aspect = drhi::DynamicImageAspectFlagBits(api);
		auto bordercolor = drhi::DynamicBorderColor(api);
		auto addressmode = drhi::DynamicSamplerAddressMode(api);

		uint32_t texSize = 512;

		// create brdf lut image
		{
			_rhiContext->createImage(&_brdflutImage, texSize, texSize, format.FORMAT_R16G16_SFLOAT, tilling.IMAGE_TILING_OPTIMAL, usage.IMAGE_USAGE_COLOR_ATTACHMENT_BIT | usage.IMAGE_USAGE_SAMPLED_BIT, samples.SAMPLE_COUNT_1_BIT, memory.MEMORY_PROPERTY_DEVICE_LOCAL_BIT,&_brdflutImageMemory);

			// create brdf lut image view
			_rhiContext->createImageView(&_brdflutImageView, &_brdflutImage, format.FORMAT_R16G16_SFLOAT, aspect.IMAGE_ASPECT_COLOR_BIT);

			// create brdf lut image sampler
			drhi::DynamicSamplerCreateInfo sci{};
			sci.borderColor = bordercolor.BORDER_COLOR_FLOAT_OPAQUE_WHITE;
			sci.sampleraAddressMode = addressmode.SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			_rhiContext->createSampler(&_brdflutSampler, sci);
		}

		// prepare resources for creating render pass
		auto loadOp = drhi::DynamicAttachmentLoadOp(api);
		auto storeOp = drhi::DynamicAttachmentStoreOp(api);
		auto layout = drhi::DynamicImageLayout(api);

		// prepare for attachment
		drhi::DynamicAttachmentDescription ad{};
		ad.format = format.FORMAT_R16G16_SFLOAT;
		ad.samples = samples.SAMPLE_COUNT_1_BIT;
		ad.loadOp = loadOp.ATTACHMENT_LOAD_OP_CLEAR;
		ad.storeOp = storeOp.ATTACHMENT_STORE_OP_STORE;
		ad.stencilLoadOp = loadOp.ATTACHMENT_LOAD_OP_DONT_CARE;
		ad.stencilStoreOp = storeOp.ATTACHMENT_STORE_OP_DONT_CARE;
		ad.initialLayout = layout.IMAGE_LAYOUT_UNDEFINED;
		ad.finalLayout = layout.IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		drhi::DynamicAttachmentReference colorRef{};
		colorRef.attachment = 0;
		colorRef.layout = layout.IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		auto bindPoint = drhi::DynamicPipelineBindPoint(api);
		drhi::DynamicSubpassDescription subpassDescription{};
		subpassDescription.pipelineBindPoint = bindPoint.PIPELINE_BIND_POINT_GRAPHICS;
		subpassDescription.colorAttachmentCount = 1;
		subpassDescription.pColorAttachments = &colorRef;

		auto stage = drhi::DynamicPipelineStageFlags(api);
		auto access = drhi::DynamicAccessFlagBits(api);
		auto dependcyFlag = drhi::DynamicDependencyFlagBits(api);
		std::vector<drhi::DynamicSubpassDependency> dependencies{2};
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
		drhi::DynamicRenderPassCreateInfo rpcreateInfo{};
		rpcreateInfo.attachmentCount = 1;
		rpcreateInfo.pAttachments = &ad;
		rpcreateInfo.subpassCount = 1;
		rpcreateInfo.pSubpasses = &subpassDescription;
		rpcreateInfo.dependencyCount = 2;
		rpcreateInfo.pDependencies = &dependencies;

		drhi::DynamicRenderPass renderPass{};
		_rhiContext->createRenderPass(&renderPass, &rpcreateInfo);

		// create framebuffer
		drhi::DynamicFramebufferCreateInfo fcreateInfo{};
		fcreateInfo.renderPass = renderPass;
		fcreateInfo.attachmentCount = 1;
		fcreateInfo.pAttachments = &_brdflutImageView;
		fcreateInfo.width = texSize;
		fcreateInfo.height = texSize;
		fcreateInfo.layers = 1;

		drhi::DynamicFramebuffer framebuffer{};
		_rhiContext->createFramebuffer(&framebuffer, &fcreateInfo);

		// descriptors
		drhi::DynamicDescriptorSetLayout dsl{nullptr};
		std::vector<drhi::DynamicDescriptorSetLayoutBinding> bindings{};
		_rhiContext->createDescriptorSetLayout(&dsl, &bindings);

		// descriptor pool
		auto desciptorType = drhi::DynamicDescriptorType(api);
		std::vector<drhi::DynamicDescriptorPoolSize> poolSizes{ 1 };
		poolSizes[0].descriptorCount = 1;
		poolSizes[0].type = desciptorType.DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

		drhi::DynamicDescriptorPoolCreateInfo pci{};
		pci.pPoolSizes = &poolSizes;
		pci.maxSets = 2;

		drhi::DynamicDescriptorPool desciptorPool;
		_rhiContext->createDescriptorPool(&desciptorPool, &pci);

		// descriptor sets
		drhi::DynamicDescriptorSet desciptorSet;
		_rhiContext->createDescriptorSet(&desciptorSet, &dsl, &desciptorPool, nullptr, 1);

		// pipeline layout
		drhi::DynamicPipelineLayout pipelineLayout{};
		drhi::DynamicPipelineLayoutCreateInfo plci{};
		plci.pPushConstantRanges = 0;
		plci.pushConstantRangeCount = 0;
		plci.pSetLayouts = &dsl;
		plci.setLayoutCount = 1;
		_rhiContext->createPipelineLayout(&pipelineLayout, &plci);

		// pipeline
		auto cullmode = drhi::DynamicCullMode(api);
		drhi::DynamicPipelineCreateInfo pipelineci{};
		pipelineci.vertexShader = RESOURCE_PATH"Shaders/IBL/brdflutVertex.spv";
		pipelineci.fragmentShader = RESOURCE_PATH"Shaders/IBL/brdflutFragment.spv";
		pipelineci.vertexInputBinding = drhi::DynamicVertexInputBindingDescription();
		pipelineci.vertexInputBinding.set(api, 0, sizeof(Vertex));
		pipelineci.vertexInputAttributes = std::vector<drhi::DynamicVertexInputAttributeDescription>();
		pipelineci.colorImageFormat = format.FORMAT_R16G16_SFLOAT;
		pipelineci.depthImageFormat = format.FORMAT_UNDEFINED;
		pipelineci.includeStencil = false;
		pipelineci.dynamicDepthBias = false;
		pipelineci.cullMode = cullmode.CULL_MODE_NONE;
		pipelineci.sampleCounts = samples.SAMPLE_COUNT_1_BIT;
		pipelineci.renderPass = &renderPass;

		drhi::DynamicPipeline pipeline;
		_rhiContext->createPipeline(&pipeline, &pipelineLayout, pipelineci);

		// start render
		drhi::DynamicRenderPassBeginInfo beginInfo{};
		beginInfo.framebuffer = framebuffer;
		beginInfo.renderPass = renderPass;
		beginInfo.renderArea.extent.width = texSize;
		beginInfo.renderArea.extent.height = texSize;

		drhi::DynamicCommandBuffer commandBuffer{};
		drhi::DynamicCommandPool commandPool{};
		_rhiContext->createCommandPool(&commandPool);
		_rhiContext->createCommandBuffer(&commandBuffer, &commandPool);
	
		_rhiContext->beginCommandBuffer(commandBuffer);

		auto content = drhi::DynamicSubpassContents(api);
		_rhiContext->beginRenderPass(&commandBuffer, &beginInfo, content.SUBPASS_CONTENTS_INLINE);
		
		drhi::DynamicViewport viewport{};
		viewport.width = texSize;
		viewport.height = texSize;
		viewport.maxDepth = 1.0f;
		viewport.minDepth = 0.0f;
		_rhiContext->cmdSetViewport(commandBuffer, 0, 1, viewport);

		drhi::DynamicRect2D scissor{};
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

	void Renderer::precomputeIrradianceCube()
	{
		auto tStart = std::chrono::high_resolution_clock::now();

		uint32_t texSize = 512;
		const uint32_t numMips = static_cast<uint32_t>(floor(std::log2(texSize))) + 1;

		auto api = _rhiContext->getCurrentAPI();
		auto format = drhi::DynamicFormat(api);
		auto tilling = drhi::DynamicImageTiling(api);
		auto usage = drhi::DynamicImageUsageFlagBits(api);
		auto samples = drhi::DynamicSampleCountFlags(api);
		auto memory = drhi::DynamicMemoryPropertyFlagBits(api);
		auto aspect = drhi::DynamicImageAspectFlagBits(api);
		auto bordercolor = drhi::DynamicBorderColor(api);
		auto addressmode = drhi::DynamicSamplerAddressMode(api);
		auto imageflags = drhi::DynamicImageCreateFlags(api);
		auto viewType = drhi::DynamicImageViewType(api);

		// create irradiance image
		{
			drhi::DynamicImageCreateInfo imageci{};
			imageci.flags = imageflags.IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
			imageci.format = format.FORMAT_R32G32B32A32_SFLOAT;
			imageci.extent.width = texSize;
			imageci.extent.height = texSize;
			imageci.extent.depth = 1;
			imageci.mipLevels = numMips;
			imageci.arrayLayers = 6;
			imageci.samples = samples.SAMPLE_COUNT_1_BIT;
			imageci.tiling = tilling.IMAGE_TILING_OPTIMAL;
			imageci.usage = usage.IMAGE_USAGE_SAMPLED_BIT | usage.IMAGE_USAGE_TRANSFER_DST_BIT;
			_rhiContext->createImage(&_irradianceImage, &_irradianceImageMemory, imageci, memory.MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

			// create irradiance image view
			drhi::DynamicImageViewCreateInfo vci{};
			vci.type = viewType.IMAGE_VIEW_TYPE_CUBE;
			vci.format = format.FORMAT_R32G32B32A32_SFLOAT;
			vci.image = _irradianceImage;
			vci.subresourceRange.aspectMask = aspect.IMAGE_ASPECT_COLOR_BIT;
			vci.subresourceRange.layerCount = 6;
			vci.subresourceRange.levelCount = numMips;
			_rhiContext->createImageView(&_irradianceImageView, &_irradianceImage, vci);

			// create irradiance image sampler
			auto mipmap = drhi::DynamicSamplerMipmapMode(api);
			drhi::DynamicSamplerCreateInfo sci{};
			sci.borderColor = bordercolor.BORDER_COLOR_FLOAT_OPAQUE_WHITE;
			sci.sampleraAddressMode = addressmode.SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			sci.mipmapMode = mipmap.SAMPLER_MIPMAP_MODE_LINEAR;
			sci.minLod = 0.0f;
			sci.maxLod = 1;
			_rhiContext->createSampler(&_irradianceSampler, sci);
		}

		// attachment
		auto loadOp = drhi::DynamicAttachmentLoadOp(api);
		auto storeOp = drhi::DynamicAttachmentStoreOp(api);
		auto layout = drhi::DynamicImageLayout(api);
		drhi::DynamicAttachmentDescription ad{};
		ad.format = format.FORMAT_R32G32B32A32_SFLOAT;
		ad.samples = samples.SAMPLE_COUNT_1_BIT;
		ad.loadOp = loadOp.ATTACHMENT_LOAD_OP_CLEAR;
		ad.storeOp = storeOp.ATTACHMENT_STORE_OP_STORE;
		ad.stencilLoadOp = loadOp.ATTACHMENT_LOAD_OP_DONT_CARE;
		ad.stencilStoreOp = storeOp.ATTACHMENT_STORE_OP_DONT_CARE;
		ad.initialLayout = layout.IMAGE_LAYOUT_UNDEFINED;
		ad.finalLayout = layout.IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		drhi::DynamicAttachmentReference ar{};
		ar.attachment = 0;
		ar.layout = layout.IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		// subpass description
		auto bindPoint = drhi::DynamicPipelineBindPoint(api);
		drhi::DynamicSubpassDescription subpassDescription{};
		subpassDescription.pipelineBindPoint = bindPoint.PIPELINE_BIND_POINT_GRAPHICS;
		subpassDescription.colorAttachmentCount = 1;
		subpassDescription.pColorAttachments = &ar;

		// subpass dependency
		auto stage = drhi::DynamicPipelineStageFlags(api);
		auto access = drhi::DynamicAccessFlagBits(api);
		auto dependcyFlag = drhi::DynamicDependencyFlagBits(api);
		std::vector<drhi::DynamicSubpassDependency> dependencies{ 2 };
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
		drhi::DynamicRenderPassCreateInfo rpcreateInfo{};
		rpcreateInfo.attachmentCount = 1;
		rpcreateInfo.pAttachments = &ad;
		rpcreateInfo.subpassCount = 1;
		rpcreateInfo.pSubpasses = &subpassDescription;
		rpcreateInfo.dependencyCount = 2;
		rpcreateInfo.pDependencies = &dependencies;

		drhi::DynamicRenderPass renderPass{};
		_rhiContext->createRenderPass(&renderPass, &rpcreateInfo);

		// create offscreen image
		drhi::DynamicImage        irradianceOffscreenImage{};
		drhi::DynamicDeviceMemory irradianceOffscreenImageMemory{};
		drhi::DynamicImageView    irradianceOffscreenImageView{};
		drhi::DynamicSampler      irradianceOffscreenSampler{};

		drhi::DynamicImageCreateInfo ici{};
		ici.format = format.FORMAT_R32G32B32A32_SFLOAT;
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

		drhi::DynamicImageViewCreateInfo ivci{};
		ivci.format = format.FORMAT_R32G32B32A32_SFLOAT;
		ivci.image = irradianceOffscreenImage;
		ivci.type = viewType.IMAGE_VIEW_TYPE_2D;
		ivci.subresourceRange.aspectMask = aspect.IMAGE_ASPECT_COLOR_BIT;
		ivci.subresourceRange.baseMipLevel = 0;
		ivci.subresourceRange.levelCount = 1;
		ivci.subresourceRange.baseArrayLayer = 0;
		ivci.subresourceRange.layerCount = 1;

		_rhiContext->createImageView(&irradianceOffscreenImageView, &irradianceOffscreenImage, ivci);

		// create framebuffer
		drhi::DynamicFramebufferCreateInfo fcreateInfo{};
		fcreateInfo.renderPass = renderPass;
		fcreateInfo.attachmentCount = 1;
		fcreateInfo.pAttachments = &irradianceOffscreenImageView;
		fcreateInfo.width = texSize;
		fcreateInfo.height = texSize;
		fcreateInfo.layers = 1;

		drhi::DynamicFramebuffer framebuffer{};
		_rhiContext->createFramebuffer(&framebuffer, &fcreateInfo);

		// set image layout
		drhi::DynamicCommandPool commandPool{};
		_rhiContext->createCommandPool(&commandPool);

		// once commandbuffer
		drhi::DynamicCommandBuffer onceCmdBuf{};
		_rhiContext->createCommandBuffer(&onceCmdBuf, &commandPool);

		_rhiContext->beginCommandBuffer(onceCmdBuf);

		_rhiContext->setImageLayout(&onceCmdBuf, &irradianceOffscreenImage, aspect.IMAGE_ASPECT_COLOR_BIT,
			layout.IMAGE_LAYOUT_UNDEFINED, layout.IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
		_rhiContext->flushCommandBuffer(onceCmdBuf, commandPool, true);

		// descriptors
		auto descriptorType = drhi::DynamicDescriptorType(api);
		auto stageFlags = drhi::DynamicShaderStageFlags(api);
		drhi::DynamicDescriptorSetLayout dsl{ nullptr };
		std::vector<drhi::DynamicDescriptorSetLayoutBinding> dsbs(1);
		dsbs[0].binding = 0;
		dsbs[0].descriptorCount = 1;
		dsbs[0].descriptorType = descriptorType.DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		dsbs[0].pImmutableSamplers = nullptr;
		dsbs[0].stageFlags = stageFlags.SHADER_STAGE_FRAGMENT_BIT;
		_rhiContext->createDescriptorSetLayout(&dsl, &dsbs);

		// descriptor pool
		auto desciptorType = drhi::DynamicDescriptorType(api);
		std::vector<drhi::DynamicDescriptorPoolSize> poolSizes{ 1 };
		poolSizes[0].descriptorCount = 1;
		poolSizes[0].type = desciptorType.DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

		drhi::DynamicDescriptorPoolCreateInfo pci{};
		pci.pPoolSizes = &poolSizes;
		pci.maxSets = 2;

		drhi::DynamicDescriptorPool desciptorPool;
		_rhiContext->createDescriptorPool(&desciptorPool, &pci);

		// descriptor sets
		drhi::DynamicDescriptorSet desciptorSet;

		drhi::DynamicDescriptorImageInfo dii{};
		dii.imageLayout = layout.IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		dii.imageView = _environmentMap->_material->_textureImageView;
		dii.sampler = _environmentMap->_material->_textureSampler;

		std::vector<drhi::DynamicWriteDescriptorSet> wds(1);
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

		auto pushStage = drhi::DynamicShaderStageFlags(api);
		drhi::DynamicPushConstantRange pcr{};
		pcr.stageFlags = pushStage.SHADER_STAGE_VERTEX_BIT | pushStage.SHADER_STAGE_FRAGMENT_BIT;
		pcr.offset = 0;
		pcr.size = sizeof(PushBlock);

		drhi::DynamicPipelineLayout pipelineLayout{};
		drhi::DynamicPipelineLayoutCreateInfo plci{};
		plci.pPushConstantRanges = &pcr;
		plci.pushConstantRangeCount = 1;
		plci.pSetLayouts = &dsl;
		plci.setLayoutCount = 1;
		_rhiContext->createPipelineLayout(&pipelineLayout, &plci);

		// pipeline
		auto cullmode = drhi::DynamicCullMode(api);
		drhi::DynamicPipelineCreateInfo pipelineci{};
		pipelineci.vertexShader = RESOURCE_PATH"Shaders/IBL/prefilterCubeVertex.spv";
		pipelineci.fragmentShader = RESOURCE_PATH"Shaders/IBL/irradianceCubeFragment.spv";
		pipelineci.vertexInputBinding = drhi::DynamicVertexInputBindingDescription();
		pipelineci.vertexInputBinding.set(api, 0, sizeof(Vertex));
		pipelineci.vertexInputAttributes = std::vector<drhi::DynamicVertexInputAttributeDescription>();
		pipelineci.vertexInputAttributes.resize(1);
		pipelineci.vertexInputAttributes[0].set(api, 0, 0, format.FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Vertex::pos));
		pipelineci.colorImageFormat = format.FORMAT_R32G32B32A32_SFLOAT;
		pipelineci.depthImageFormat = format.FORMAT_UNDEFINED;
		pipelineci.includeStencil = false;
		pipelineci.dynamicDepthBias = false;
		pipelineci.cullMode = cullmode.CULL_MODE_NONE;
		pipelineci.sampleCounts = samples.SAMPLE_COUNT_1_BIT;
		pipelineci.renderPass = &renderPass;

		drhi::DynamicPipeline pipeline;
		_rhiContext->createPipeline(&pipeline, &pipelineLayout, pipelineci);

		// rendering
		auto content = drhi::DynamicSubpassContents(api);
		drhi::DynamicRenderPassBeginInfo binfo{};
		binfo.framebuffer = framebuffer;
		binfo.renderPass = renderPass;
		binfo.renderArea.extent.width = texSize;
		binfo.renderArea.extent.height = texSize;
		binfo.renderArea.offset.x = 0;
		binfo.renderArea.offset.y = 0;

		// prepare command buffer
		drhi::DynamicCommandBuffer commandBuffer{};
		_rhiContext->createCommandBuffer(&commandBuffer, &commandPool);

		_rhiContext->beginCommandBuffer(commandBuffer);

		// scissor
		drhi::DynamicViewport viewport{};
		viewport.width = texSize;
		viewport.height = texSize;
		viewport.maxDepth = 1.0f;
		viewport.minDepth = 0.0f;
		_rhiContext->cmdSetViewport(commandBuffer, 0, 1, viewport);

		drhi::DynamicRect2D scissor{};
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
		drhi::DynamicImageSubresourceRange range{};
		range.aspectMask = aspect.IMAGE_ASPECT_COLOR_BIT;
		range.baseMipLevel = 0;
		range.levelCount = numMips;
		range.layerCount = 6;
		_rhiContext->setImageLayout(&commandBuffer, &_irradianceImage, layout.IMAGE_LAYOUT_UNDEFINED, layout.IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, range);

		// start render pass
		for (uint32_t level = 0; level < numMips; ++level)
		{
			for (uint32_t f = 0; f < 6; ++f)
			{
				viewport.width = static_cast<float>(texSize * std::pow(0.5f, level));
				viewport.height = static_cast<float>(texSize * std::pow(0.5f, level));
				_rhiContext->cmdSetViewport(commandBuffer, 0, 1, viewport);

				_rhiContext->beginRenderPass(&commandBuffer, &binfo, content.SUBPASS_CONTENTS_INLINE);

				pushBlock.mvp = perspective((float)(PI / 2.0), 1.0f, 0.1f, 512.0f) * matrices[f];

				_rhiContext->cmdPushConstants(&pipelineLayout, &commandBuffer, pushStage.SHADER_STAGE_VERTEX_BIT | pushStage.SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushBlock), &pushBlock);

				_environmentMap->draw(_rhiContext, &commandBuffer, pipeline, pipelineLayout, desciptorSet);

				_rhiContext->endRenderPass(&commandBuffer);

				_rhiContext->setImageLayout(&commandBuffer, &irradianceOffscreenImage, aspect.IMAGE_ASPECT_COLOR_BIT, layout.IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, layout.IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

				drhi::DynamicImageCopy copy{};
				copy.srcSubresource.aspectMask = aspect.IMAGE_ASPECT_COLOR_BIT;
				copy.srcSubresource.baseArrayLayer = 0;
				copy.srcSubresource.mipLevel = 0;
				copy.srcSubresource.layerCount = 1;
				copy.srcOffset = { 0,0,0 };

				copy.dstSubresource.aspectMask = aspect.IMAGE_ASPECT_COLOR_BIT;
				copy.dstSubresource.baseArrayLayer = f;
				copy.dstSubresource.mipLevel = level;
				copy.dstSubresource.layerCount = 1;
				copy.dstOffset = { 0,0,0 };

				copy.extent = { (uint32_t)viewport.width, (uint32_t)viewport.height, 1 };

				_rhiContext->cmdCopyImage(commandBuffer, &irradianceOffscreenImage, layout.IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, &_irradianceImage, layout.IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, copy);

				_rhiContext->setImageLayout(&commandBuffer, &irradianceOffscreenImage, aspect.IMAGE_ASPECT_COLOR_BIT, layout.IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, layout.IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
			}
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

	void Renderer::prefilterEnvironmentCube()
	{
		auto tStart = std::chrono::high_resolution_clock::now();

		uint32_t texSize = 1024;
		const uint32_t numMips = static_cast<uint32_t>(floor(std::log2(texSize))) + 1;

		auto api = _rhiContext->getCurrentAPI();
		auto format = drhi::DynamicFormat(api);
		auto tilling = drhi::DynamicImageTiling(api);
		auto usage = drhi::DynamicImageUsageFlagBits(api);
		auto samples = drhi::DynamicSampleCountFlags(api);
		auto memory = drhi::DynamicMemoryPropertyFlagBits(api);
		auto aspect = drhi::DynamicImageAspectFlagBits(api);
		auto bordercolor = drhi::DynamicBorderColor(api);
		auto addressmode = drhi::DynamicSamplerAddressMode(api);
		auto imageflags = drhi::DynamicImageCreateFlags(api);
		auto viewType = drhi::DynamicImageViewType(api);

		// create image
		{
			drhi::DynamicImageCreateInfo imageCI{};
			imageCI.format = format.FORMAT_R16G16B16A16_SFLOAT;
			imageCI.extent.width = texSize;
			imageCI.extent.height = texSize;
			imageCI.extent.depth = 1;
			imageCI.mipLevels = numMips;
			imageCI.arrayLayers = 6;
			imageCI.samples = samples.SAMPLE_COUNT_1_BIT;
			imageCI.tiling = tilling.IMAGE_TILING_OPTIMAL;
			imageCI.usage = usage.IMAGE_USAGE_SAMPLED_BIT | usage.IMAGE_USAGE_TRANSFER_DST_BIT;
			imageCI.flags = imageflags.IMAGE_CREATE_CUBE_COMPATIBLE_BIT;

			_rhiContext->createImage(&_filteredImage, &_filteredImageMemory, imageCI, memory.MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		
			drhi::DynamicImageViewCreateInfo viewInfo{};
			viewInfo.format = format.FORMAT_R16G16B16A16_SFLOAT;
			viewInfo.image = _filteredImage;
			viewInfo.type = viewType.IMAGE_VIEW_TYPE_CUBE;
			viewInfo.subresourceRange.aspectMask = aspect.IMAGE_ASPECT_COLOR_BIT;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.layerCount = 6;
			viewInfo.subresourceRange.levelCount = numMips;

			_rhiContext->createImageView(&_filteredImageView, &_filteredImage, viewInfo);

			auto mipmap = drhi::DynamicSamplerMipmapMode(api);
			drhi::DynamicSamplerCreateInfo samplerInfo{};
			samplerInfo.borderColor = bordercolor.BORDER_COLOR_FLOAT_OPAQUE_WHITE;
			samplerInfo.maxLod = 1;
			samplerInfo.minLod = 0.0f;
			samplerInfo.mipmapMode = mipmap.SAMPLER_MIPMAP_MODE_LINEAR;
			samplerInfo.sampleraAddressMode = addressmode.SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

			_rhiContext->createSampler(&_filteredImageSampler, samplerInfo);
		}

		// color attachment
		auto loadOp = drhi::DynamicAttachmentLoadOp(api);
		auto storeOp = drhi::DynamicAttachmentStoreOp(api);
		auto layout = drhi::DynamicImageLayout(api);
		drhi::DynamicAttachmentDescription ad{};
		ad.format = format.FORMAT_R16G16B16A16_SFLOAT;
		ad.samples = samples.SAMPLE_COUNT_1_BIT;
		ad.loadOp = loadOp.ATTACHMENT_LOAD_OP_CLEAR;
		ad.storeOp = storeOp.ATTACHMENT_STORE_OP_STORE;
		ad.stencilLoadOp = loadOp.ATTACHMENT_LOAD_OP_DONT_CARE;
		ad.stencilStoreOp = storeOp.ATTACHMENT_STORE_OP_DONT_CARE;
		ad.initialLayout = layout.IMAGE_LAYOUT_UNDEFINED;
		ad.finalLayout = layout.IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		drhi::DynamicAttachmentReference ar{};
		ar.attachment = 0;
		ar.layout = layout.IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		
		// subpass description
		auto bindPoint = drhi::DynamicPipelineBindPoint(api);
		drhi::DynamicSubpassDescription subpassDescription{};
		subpassDescription.pipelineBindPoint = bindPoint.PIPELINE_BIND_POINT_GRAPHICS;
		subpassDescription.colorAttachmentCount = 1;
		subpassDescription.pColorAttachments = &ar;

		// subpass dependencies
		auto stage = drhi::DynamicPipelineStageFlags(api);
		auto access = drhi::DynamicAccessFlagBits(api);
		auto dependcyFlag = drhi::DynamicDependencyFlagBits(api);
		std::vector<drhi::DynamicSubpassDependency> dependencies{ 2 };
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
		drhi::DynamicRenderPassCreateInfo renderPassCI{};
		renderPassCI.attachmentCount = 1;
		renderPassCI.pAttachments = &ad;
		renderPassCI.subpassCount = 1;
		renderPassCI.pSubpasses = &subpassDescription;
		renderPassCI.dependencyCount = 2;
		renderPassCI.pDependencies = &dependencies;

		drhi::DynamicRenderPass renderPass{};
		_rhiContext->createRenderPass(&renderPass, &renderPassCI);

		// create offscreen image
		drhi::DynamicImage        offscreenImage{};
		drhi::DynamicDeviceMemory offscreenImageMemory{};
		drhi::DynamicImageView    offscreenImageView{};
		drhi::DynamicSampler      offscreenSampler{};

		drhi::DynamicImageCreateInfo ici{};
		ici.format = format.FORMAT_R16G16B16A16_SFLOAT;
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

		drhi::DynamicImageViewCreateInfo ivci{};
		ivci.format = format.FORMAT_R16G16B16A16_SFLOAT;
		ivci.image = offscreenImage;
		ivci.type = viewType.IMAGE_VIEW_TYPE_2D;
		ivci.subresourceRange.aspectMask = aspect.IMAGE_ASPECT_COLOR_BIT;
		ivci.subresourceRange.baseMipLevel = 0;
		ivci.subresourceRange.levelCount = 1;
		ivci.subresourceRange.baseArrayLayer = 0;
		ivci.subresourceRange.layerCount = 1;

		_rhiContext->createImageView(&offscreenImageView, &offscreenImage, ivci);

		// create framebuffer
		drhi::DynamicFramebufferCreateInfo fcreateInfo{};
		fcreateInfo.renderPass = renderPass;
		fcreateInfo.attachmentCount = 1;
		fcreateInfo.pAttachments = &offscreenImageView;
		fcreateInfo.width = texSize;
		fcreateInfo.height = texSize;
		fcreateInfo.layers = 1;

		drhi::DynamicFramebuffer framebuffer{};
		_rhiContext->createFramebuffer(&framebuffer, &fcreateInfo);

		drhi::DynamicCommandPool cmdPool{};
		drhi::DynamicCommandBuffer cmdBuffer{};
		_rhiContext->createCommandPool(&cmdPool);
		_rhiContext->createCommandBuffer(&cmdBuffer, &cmdPool);
		_rhiContext->beginCommandBuffer(cmdBuffer);
		_rhiContext->setImageLayout(&cmdBuffer, &offscreenImage,aspect.IMAGE_ASPECT_COLOR_BIT, layout.IMAGE_LAYOUT_UNDEFINED, layout.IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
		_rhiContext->flushCommandBuffer(cmdBuffer, cmdPool, true);

		// descriptors
		auto descriptorType = drhi::DynamicDescriptorType(api);
		auto stageFlags = drhi::DynamicShaderStageFlags(api);
		drhi::DynamicDescriptorSetLayout dsl{ nullptr };
		std::vector<drhi::DynamicDescriptorSetLayoutBinding> dsbs(1);
		dsbs[0].binding = 0;
		dsbs[0].descriptorCount = 1;
		dsbs[0].descriptorType = descriptorType.DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		dsbs[0].pImmutableSamplers = nullptr;
		dsbs[0].stageFlags = stageFlags.SHADER_STAGE_FRAGMENT_BIT;
		_rhiContext->createDescriptorSetLayout(&dsl, &dsbs);

		// descriptor pool
		auto desciptorType = drhi::DynamicDescriptorType(api);
		std::vector<drhi::DynamicDescriptorPoolSize> poolSizes{ 1 };
		poolSizes[0].descriptorCount = 1;
		poolSizes[0].type = desciptorType.DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

		drhi::DynamicDescriptorPoolCreateInfo pci{};
		pci.pPoolSizes = &poolSizes;
		pci.maxSets = 2;

		drhi::DynamicDescriptorPool desciptorPool;
		_rhiContext->createDescriptorPool(&desciptorPool, &pci);

		// descriptor sets
		drhi::DynamicDescriptorSet desciptorSet;

		drhi::DynamicDescriptorImageInfo dii{};
		dii.imageLayout = layout.IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		dii.imageView = _environmentMap->_material->_textureImageView;
		dii.sampler = _environmentMap->_material->_textureSampler;

		std::vector<drhi::DynamicWriteDescriptorSet> wds(1);
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

		auto pushStage = drhi::DynamicShaderStageFlags(api);
		drhi::DynamicPushConstantRange pcr{};
		pcr.stageFlags = pushStage.SHADER_STAGE_VERTEX_BIT | pushStage.SHADER_STAGE_FRAGMENT_BIT;
		pcr.offset = 0;
		pcr.size = sizeof(PushBlock);

		drhi::DynamicPipelineLayout pipelineLayout{};
		drhi::DynamicPipelineLayoutCreateInfo plci{};
		plci.pPushConstantRanges = &pcr;
		plci.pushConstantRangeCount = 1;
		plci.pSetLayouts = &dsl;
		plci.setLayoutCount = 1;
		_rhiContext->createPipelineLayout(&pipelineLayout, &plci);

		// pipeline
		auto cullmode = drhi::DynamicCullMode(api);
		drhi::DynamicPipelineCreateInfo pipelineci{};
		pipelineci.vertexShader = RESOURCE_PATH"Shaders/IBL/prefilterCubeVertex.spv";
		pipelineci.fragmentShader = RESOURCE_PATH"Shaders/IBL/prefilterEnvmapFragment.spv";
		pipelineci.vertexInputBinding = drhi::DynamicVertexInputBindingDescription();
		pipelineci.vertexInputBinding.set(api, 0, sizeof(Vertex));
		pipelineci.vertexInputAttributes = std::vector<drhi::DynamicVertexInputAttributeDescription>();
		pipelineci.vertexInputAttributes.resize(1);
		pipelineci.vertexInputAttributes[0].set(api, 0, 0, format.FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Vertex::pos));
		pipelineci.colorImageFormat = format.FORMAT_R16G16B16A16_SFLOAT;
		pipelineci.depthImageFormat = format.FORMAT_UNDEFINED;
		pipelineci.includeStencil = false;
		pipelineci.dynamicDepthBias = false;
		pipelineci.cullMode = cullmode.CULL_MODE_NONE;
		pipelineci.sampleCounts = samples.SAMPLE_COUNT_1_BIT;
		pipelineci.renderPass = &renderPass;

		drhi::DynamicPipeline pipeline{};
		_rhiContext->createPipeline(&pipeline, &pipelineLayout, pipelineci);

		// begin render pass
		drhi::DynamicRenderPassBeginInfo rbinfo{};
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

		drhi::DynamicCommandBuffer commandBuffer{};
		_rhiContext->createCommandBuffer(&commandBuffer, &cmdPool);
		_rhiContext->beginCommandBuffer(commandBuffer);

		drhi::DynamicViewport viewPort{};
		viewPort.width = texSize;
		viewPort.height = texSize;
		viewPort.minDepth = 0.0f;
		viewPort.maxDepth = 1.0f;
		viewPort.x = 0;
		viewPort.y = 0;

		_rhiContext->cmdSetViewport(commandBuffer, 0, 1, viewPort);

		drhi::DynamicRect2D scissor{};
		scissor.extent.width = texSize;
		scissor.extent.height = texSize;
		scissor.offset.x = 0;
		scissor.offset.y = 0;
		_rhiContext->cmdSetScissor(commandBuffer, 0, 1, scissor);

		drhi::DynamicImageSubresourceRange range{};
		range.aspectMask = aspect.IMAGE_ASPECT_COLOR_BIT;
		range.baseMipLevel = 0;
		range.levelCount = numMips;
		range.layerCount = 6;
		_rhiContext->setImageLayout(&commandBuffer, &_filteredImage, layout.IMAGE_LAYOUT_UNDEFINED, layout.IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, range);

		auto content = drhi::DynamicSubpassContents(api);
		auto shaderStage = drhi::DynamicShaderStageFlags(api);
		// rendering
		for (uint32_t m = 0; m < numMips; m++) {
			pushBlock.roughness = (float)m / (float)(numMips - 1);
			for (uint32_t f = 0; f < 6; ++f)
			{
				viewPort.width = static_cast<float>(texSize * std::pow(0.5f, m));
				viewPort.height = static_cast<float>(texSize * std::pow(0.5f, m));

				_rhiContext->cmdSetViewport(commandBuffer, 0, 1, viewPort);

				_rhiContext->beginRenderPass(&commandBuffer, &rbinfo, content.SUBPASS_CONTENTS_INLINE);

				pushBlock.mvp = perspective((float)(PI / 2.0), 1.0f, 0.1f, 512.f) * matrices[f];

				_rhiContext->cmdPushConstants(&pipelineLayout, &commandBuffer, shaderStage.SHADER_STAGE_VERTEX_BIT | shaderStage.SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushBlock), &pushBlock);

				_rhiContext->bindPipeline(pipeline, &commandBuffer, bindPoint.PIPELINE_BIND_POINT_GRAPHICS);
				_rhiContext->bindDescriptorSets(&desciptorSet, pipelineLayout, &commandBuffer, bindPoint.PIPELINE_BIND_POINT_GRAPHICS);

				_environmentMap->draw(_rhiContext, &commandBuffer, pipeline, pipelineLayout, desciptorSet);

				_rhiContext->endRenderPass(&commandBuffer);

				_rhiContext->setImageLayout(&commandBuffer, &offscreenImage, aspect.IMAGE_ASPECT_COLOR_BIT, layout.IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, layout.IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

				drhi::DynamicImageCopy copy{};
				copy.srcSubresource.aspectMask = aspect.IMAGE_ASPECT_COLOR_BIT;
				copy.srcSubresource.baseArrayLayer = 0;
				copy.srcSubresource.mipLevel = 0;
				copy.srcSubresource.layerCount = 1;
				copy.srcOffset = { 0,0,0 };

				copy.dstSubresource.aspectMask = aspect.IMAGE_ASPECT_COLOR_BIT;
				copy.dstSubresource.baseArrayLayer = f;
				copy.dstSubresource.mipLevel = m;
				copy.dstSubresource.layerCount = 1;
				copy.dstOffset = { 0,0,0 };

				copy.extent = { (uint32_t)viewPort.width, (uint32_t)viewPort.height, 1};

				_rhiContext->cmdCopyImage(commandBuffer, &offscreenImage, layout.IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, &_filteredImage, layout.IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, copy);

				_rhiContext->setImageLayout(&commandBuffer, &offscreenImage, aspect.IMAGE_ASPECT_COLOR_BIT, layout.IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, layout.IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
			}
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