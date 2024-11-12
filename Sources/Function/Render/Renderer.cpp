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

	void Renderer::buildAndSubmit(std::vector<std::shared_ptr<RenderResource>> renderlist)
	{
		_submitRenderlist = renderlist;

		for (auto p : _submitRenderlist)
		{
			p->build(_rhiContext);
		}

		buildCommandBuffer();
	}

	void Renderer::buildCommandBuffer()
	{
		for (uint32_t i = 0; i < _rhiContext->getCommandBufferSize(); ++i)
		{
			_rhiContext->beginCommandBuffer(i);

			for (auto p : _submitRenderlist)
			{
				p->draw(i, _rhiContext);
			}

			_rhiContext->endCommandBuffer(i);
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

	
}