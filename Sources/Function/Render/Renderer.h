#pragma once

#include<memory>
#include<vector>

#include<drhi.h>

#include"../../Core/Math.h"
#include"RenderResource.h"

namespace FOCUS
{
	class Renderer
	{
	public:
		std::shared_ptr<DRHI::DynamicRHI> _rhiContext;
		std::vector<std::shared_ptr<RenderResource>> _submitRenderlist;
		
		std::vector<DRHI::DynamicCommandBuffer> _commandBuffers;
		DRHI::DynamicCommandPool _commandPool;

		std::vector<DRHI::DynamicImage>* _viewportImages{nullptr};
		std::vector<DRHI::DynamicImageView>* _viewportImageViews{nullptr};

		bool _prepared = false;

	public:
		Renderer() = delete;

		Renderer(DRHI::API api, DRHI::PlatformInfo platformCI);
		
		void initialize();
		void buildCommandBuffer();
		void buildAndSubmit(std::vector<std::shared_ptr<RenderResource>>* renderlist, std::vector<DRHI::DynamicCommandBuffer>* commandBuffers, DRHI::DynamicCommandPool* commandPool);
		void submitRenderTargetImage(std::vector<DRHI::DynamicImage>* viewportImages, std::vector<DRHI::DynamicImageView>* viewportImageViews);
		void clean();
		void recreate();
	};
}