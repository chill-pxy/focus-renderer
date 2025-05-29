#include"RenderSystem.h"

#include<ffx_api/ffx_api_types.h>

namespace focus
{
	void RenderSystem::initialize(RenderSystemCreateInfo rsci)
	{
		drhi::PlatformInfo platformCI{};
		platformCI.window = rsci.window;
		platformCI.width = rsci.width;
		platformCI.height = rsci.height;

		// count max thread
		_maxThread = std::thread::hardware_concurrency();

		// initialize renderer
		_renderer = std::make_shared<Renderer>(drhi::VULKAN, platformCI);
		_renderer->initialize();

		// create primary commandbuffer
		_renderer->_rhiContext->createCommandPool(&_priCmdpool);
		_renderer->_rhiContext->createCommandBuffers(&_priCmdbuf, &_priCmdpool);

		// initialize scene
		_scene = std::make_shared<RenderScene>();
		_scene->initialize(_renderer->_rhiContext);

		// initialize fsr
		initializeFSR();

		// record command list
		recordCommand(_priCmdbuf);
		recordCommand(_renderer->_shadowCommandBuffers);
		recordCommand(_renderer->_sceneCommandBuffers);

		_isInitialized = true;
	}

	void RenderSystem::tick(bool* running)
	{
		if (!_isInitialized) return; 
		if (!*running) return;

		auto tStart = std::chrono::high_resolution_clock::now();

		// renderer tick
		_renderer->_rhiContext->frameOnTick(_recreateFunc, &_submitCommandBuffers);
		
		// compute time on every frame cost
		_frameCounter++;
		
		auto tEnd = std::chrono::high_resolution_clock::now();
		auto tDiff = std::chrono::duration<double, std::milli>(tEnd - tStart).count();
		
		_frameTimer = (float)tDiff / 1000.0f;

		// scene tick
		_scene->tick(_frameTimer);
		_renderer->_environmentMap->updateUniformBuffer(_scene->_uud);

		// Convert to clamped timer value
		_timer += _timerSpeed * _frameTimer;
		if (_timer > 1.0)
		{
			_timer -= 1.0f;
		}

		float fpsTimer = (float)(std::chrono::duration<double, std::milli>(tEnd - _lastTimestamp).count());
		if (fpsTimer > 1000.0f)
		{
			_lastFPS = static_cast<uint32_t>((float)_frameCounter * (1000.0f / fpsTimer));

			_frameCounter = 0;
			_lastTimestamp = tEnd;
		}
		_tPrevEnd = tEnd;

		*running = true;
	}

	void RenderSystem::update(std::function<void(uint32_t)> job, drhi::DynamicCommandBufferInheritanceInfo& inheritanceInfo, std::vector<drhi::DynamicCommandBuffer> exCmdbuf)
	{
		auto api = _renderer->_rhiContext->getCurrentAPI();
		auto imageLayout = drhi::DynamicImageLayout(api);
		auto format = drhi::DynamicFormat(api);
		auto samples = drhi::DynamicSampleCountFlags(api);

		drhi::DynamicRenderingInfo renderInfo{};
		renderInfo.isRenderOnSwapChain = true;
		renderInfo.isClearEveryFrame = true;
		renderInfo.depthImageLayout = imageLayout.IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		//if (inheritanceInfo.pNext == nullptr)
		{
			inheritanceInfo.framebuffer = nullptr;
			inheritanceInfo.renderPass = nullptr;
			inheritanceInfo.subpass = 0;
			
			drhi::DynamicCommandBufferInheritanceRenderingInfoKHR inheritanceInfoKHR{};
			uint32_t colorFormat = format.FORMAT_B8G8R8A8_UNORM;
			inheritanceInfoKHR.colorAttachmentCount = 1;
			inheritanceInfoKHR.depthAttachmentFormat = format.FORMAT_D32_SFLOAT_S8_UINT;
			inheritanceInfoKHR.pColorAttachmentFormats = &colorFormat;
			inheritanceInfoKHR.stencilAttachmentFormat = format.FORMAT_UNDEFINED;
			inheritanceInfoKHR.rasterizationSamples = samples.SAMPLE_COUNT_1_BIT;
			renderInfo.isRenderBySecondaryCommand = true;

			inheritanceInfo.pNext = &inheritanceInfoKHR;
		}

		for (uint32_t index = 0; index < _priCmdbuf.size(); ++index)
		{
			renderInfo.swapChainIndex = index;

			_renderer->_rhiContext->beginCommandBuffer(_priCmdbuf[index]);
			_renderer->_rhiContext->beginRendering(_priCmdbuf[index], renderInfo);

			job(index);

			_renderer->_rhiContext->executeCommands(_priCmdbuf[index], std::vector<drhi::DynamicCommandBuffer>{exCmdbuf[index]});

			_renderer->_rhiContext->endRendering(_priCmdbuf[index], renderInfo);
			_renderer->_rhiContext->endCommandBuffer(_priCmdbuf[index]);
		}
	}

	void RenderSystem::recordCommand(std::vector<drhi::DynamicCommandBuffer> cmdbufs)
	{
		_submitCommandBuffers.insert(_submitCommandBuffers.end(), cmdbufs.begin(), cmdbufs.end());
	}

	void RenderSystem::clean()
	{
		_renderer->_rhiContext->freeCommandBuffers(&_priCmdbuf, &_priCmdpool);
		_renderer->_rhiContext->destroyCommandPool(&_priCmdpool);

		_scene->clean(_renderer->_rhiContext);
		_renderer->clean();
		_isInitialized = false;
	}

	void RenderSystem::setViewportSize(uint32_t width, uint32_t height)
	{
		if (_renderer)
		{
			auto vkrhi = static_cast<drhi::VulkanDRHI*>(_renderer->_rhiContext.get());
			vkrhi->_viewPortWidth = width;
			vkrhi->_viewPortHeight = height;
		}
	}

	void RenderSystem::setSwapChainSize(uint32_t width, uint32_t height)
	{
		if (_renderer)
		{
			auto vkrhi = static_cast<drhi::VulkanDRHI*>(_renderer->_rhiContext.get());
			vkrhi->_swapChainExtent.width = width;
			vkrhi->_swapChainExtent.height = height;
		}
	}

	void RenderSystem::build()
	{
		_renderer->buildAndSubmit(&_scene->_submitGroup);
	}

	std::shared_ptr<Renderer> RenderSystem::getRenderer()
	{
		return _renderer;
	}

	void RenderSystem::updateRenderCanvasSize(uint32_t width, uint32_t height)
	{
		_renderWidth = width;
		_renderHeight = height;

		_scene->_camera->_viewportWidth = width;
		_scene->_camera->_viewportHeight = height;
		_scene->_camera->updateProjMatrix();
	}

	void RenderSystem::initializeFSR()
	{
		// experiment for ffx
	
		// initialize ffx context
		if (_renderer->_rhiContext->getCurrentAPI() == drhi::VULKAN)
		{
			ffx::CreateBackendVKDesc backend{};
			backend.vkDevice = static_cast<drhi::VulkanDRHI*>(_renderer->_rhiContext.get())->_device;
			backend.vkPhysicalDevice = static_cast<drhi::VulkanDRHI*>(_renderer->_rhiContext.get())->_physicalDevice;
			backend.vkDeviceProcAddr = *(static_cast<drhi::VulkanDRHI*>(_renderer->_rhiContext.get())->getVkDeviceProcAddr());
			backend.header.type = FFX_API_CREATE_CONTEXT_DESC_TYPE_BACKEND_VK;

			ffx::CreateContextDescUpscale upscale{};
			upscale.header.type = FFX_API_CREATE_CONTEXT_DESC_TYPE_UPSCALE;
			upscale.maxUpscaleSize = { _renderer->_rhiContext->getSwapChainExtentWidth(), _renderer->_rhiContext->getSwapChainExtentHeight() };
			upscale.maxRenderSize = { _renderer->_rhiContext->getSwapChainExtentWidth(), _renderer->_rhiContext->getSwapChainExtentHeight() };
			upscale.flags = FFX_UPSCALE_ENABLE_AUTO_EXPOSURE | FFX_UPSCALE_ENABLE_HIGH_DYNAMIC_RANGE;
			upscale.fpMessage = nullptr;

			ffx::ReturnCode result = ffx::CreateContext(_fsrContext, nullptr, upscale, backend);
			if (result != ffx::ReturnCode::Ok)
			{
				std::cout << "error";
			}
		}

		// apply camera jitter
		auto cameraJitterCallBack = [this](Vector2& values)
			{
				// increment jitter index for frame
				++_jitterIndex;

				ffx::ReturnCode                     retCode;
				int32_t                             jitterPhaseCount;
				ffx::QueryDescUpscaleGetJitterPhaseCount getJitterPhaseDesc{};
				getJitterPhaseDesc.displayWidth = _renderer->_rhiContext->getSwapChainExtentWidth();
				getJitterPhaseDesc.renderWidth = _renderWidth;
				getJitterPhaseDesc.pOutPhaseCount = &jitterPhaseCount;

				retCode = ffx::Query(_fsrContext, getJitterPhaseDesc);

				ffx::QueryDescUpscaleGetJitterOffset getJitterOffsetDesc{};
				getJitterOffsetDesc.index = _jitterIndex;
				getJitterOffsetDesc.phaseCount = jitterPhaseCount;
				getJitterOffsetDesc.pOutX = &_jitterX;
				getJitterOffsetDesc.pOutY = &_jitterY;

				retCode = ffx::Query(_fsrContext, getJitterOffsetDesc);

				values = Vector2(-2.f * _jitterX / _renderWidth, 2.f * _jitterY / _renderHeight);
			};

		_scene->_camera->setJitterCallBack(cameraJitterCallBack);

		// dispatch
		ffx::DispatchDescUpscale dispatchUpscale{};
		dispatchUpscale.commandList = _submitCommandBuffers.data();

		uint32_t state = FFX_API_RESOURCE_STATE_COMPUTE_READ;
        VkImage image = _renderer->_viewportImages->at(0).getVulkanImage();
		//dispatchUpscale.color = ffxApiGetResourceVK((void*)image, ffxApiGetImageResourceDescriptionVK(image, ), state)

	}
}