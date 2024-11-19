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
	}

	void Renderer::buildAndSubmit(std::vector<std::shared_ptr<RenderResource>> renderlist, std::vector<DRHI::DynamicCommandBuffer>* commandBuffers, DRHI::DynamicCommandPool* commandPool)
	{
		_submitRenderlist = renderlist;
		_commandBuffers = *commandBuffers;
		_commandPool = *commandPool;

		for (auto p : _submitRenderlist)
		{
			p->build(_rhiContext, &_commandPool);
		}

		buildCommandBuffer();
	}

	void Renderer::buildCommandBuffer()
	{
		//auto index = _rhiContext->getCurrentFrame();
		for (int index = 0; index < _commandBuffers.size(); ++index)
		{
			_rhiContext->beginCommandBuffer(_commandBuffers[index]);
			_rhiContext->beginRendering(_commandBuffers[index], true, index);

			for (auto p : _submitRenderlist)
			{
				p->draw(_rhiContext, &_commandBuffers[index]);
			}

			_rhiContext->endRendering(_commandBuffers[index], index);
			_rhiContext->endCommandBuffer(_commandBuffers[index]);
		}
	}

	void Renderer::clean()
	{
		_rhiContext->clean();
		/*
		for (int i = 0; i < obj->_uniformBuffers.size(); ++i)
		{
			_rhiContext->clearBuffer(&obj->_uniformBuffers[i], &obj->_uniformBuffersMemory[i]);
		}

		_rhiContext->clearBuffer(&obj->_vertexBuffer, &obj->_vertexDeviceMemory);
		_rhiContext->clearBuffer(&obj->_indexBuffer, &obj->_indexDeviceMemory);
		_rhiContext->clearImage(&obj->_textureSampler, &obj->_textureImageView, &obj->_textureImage, &obj->_textureMemory);*/
	}

	void Renderer::recreate()
	{
		//_rhiContext->freeCommandBuffers(&_commandBuffers, &_commandPool);
		_rhiContext->createCommandBuffers(&_commandBuffers, &_commandPool);
		buildCommandBuffer();
	}
}