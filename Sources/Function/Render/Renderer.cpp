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

		_shadowMap = std::make_shared<ShadowMap>();
	}

	void Renderer::initialize()
	{
		_rhiContext->initialize();

		_shadowMap->initialize(_rhiContext);

		_prepared = true;
	}

	void Renderer::buildAndSubmit(std::vector<std::shared_ptr<RenderResource>>* renderlist, std::vector<DRHI::DynamicCommandBuffer>* commandBuffers, DRHI::DynamicCommandPool* commandPool)
	{
		_submitRenderlist = *renderlist;
		_commandBuffers = *commandBuffers;
		_commandPool = *commandPool;

		for (auto p : _submitRenderlist)
		{
			p->build(_rhiContext, &_commandPool);
		}

		buildCommandBuffer();
	}

	void Renderer::submitRenderTargetImage(std::vector<DRHI::DynamicImage>* viewportImages, std::vector<DRHI::DynamicImageView>* viewportImageViews)
	{
		_viewportImages = viewportImages;
		_viewportImageViews = viewportImageViews;
	}

	void Renderer::buildCommandBuffer()
	{
		if (_prepared)
		{
			DRHI::DynamicRenderingInfo renderInfo{};
			renderInfo.isRenderOnSwapChain = false;
			renderInfo.isClearEveryFrame = true;

			// rendering shadow map
			for (int index = 0; index < _commandBuffers.size(); ++index)
			{
				renderInfo.targetImage = &(*_viewportImages)[index];
				renderInfo.targetImageView = &(*_viewportImageViews)[index];

				_rhiContext->beginCommandBuffer(_commandBuffers[index]);
				_rhiContext->beginRendering(_commandBuffers[index], renderInfo);

				// binding shadow map pipeline
				auto api = _rhiContext->getCurrentAPI();
				auto bindPoint = DRHI::DynamicPipelineBindPoint(api);

				_rhiContext->bindPipeline(_shadowMap->_shadowPipeline, &_commandBuffers[index], bindPoint.PIPELINE_BIND_POINT_GRAPHICS);
				_rhiContext->bindDescriptorSets(&_shadowMap->_descriptorSet, _shadowMap->_shadowPipelineLayout, &_commandBuffers[index], 0);

				for (auto p : _submitRenderlist)
				{
					p->draw(_rhiContext, &_commandBuffers[index], false);
				}

				_rhiContext->endRendering(_commandBuffers[index], renderInfo);
				_rhiContext->endCommandBuffer(_commandBuffers[index]);
			}

			// rendering scene
			for (int index = 0; index < _commandBuffers.size(); ++index)
			{
				renderInfo.targetImage = &(*_viewportImages)[index];
				renderInfo.targetImageView = &(*_viewportImageViews)[index];

				_rhiContext->beginCommandBuffer(_commandBuffers[index]);
				_rhiContext->beginRendering(_commandBuffers[index], renderInfo);

				for (auto p : _submitRenderlist)
				{
					p->draw(_rhiContext, &_commandBuffers[index], true);
				}

				_rhiContext->endRendering(_commandBuffers[index], renderInfo);
				_rhiContext->endCommandBuffer(_commandBuffers[index]);
			}
		}
	}

	void Renderer::tick(UniformUpdateData ubo)
	{
		_shadowMap->updateUniform(ubo);
	}

	void Renderer::clean()
	{
		_prepared = false;

		_rhiContext->clean();
	}

	void Renderer::recreate()
	{
		_prepared = true;
		buildCommandBuffer();
	}
}