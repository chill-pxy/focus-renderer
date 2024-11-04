#pragma once

#include<memory>
#include<vector>

#include<drhi.h>

#include"../../Core/Math.h"
#include"Overlay.h"

namespace FOCUS
{
	class Renderer
	{
	public:
		std::shared_ptr<DRHI::DynamicRHI> _rhiContext;
		std::vector<std::shared_ptr<RenderResource>>      _submitRenderlist;

	public:
		Renderer() = delete;

		Renderer(DRHI::API api, DRHI::PlatformInfo platformCI);
		
		void initialize();
		void buildCommandBuffer();
		void buildAndSubmit(std::vector<std::shared_ptr<RenderResource>> renderlist);
		void clean();
	};
}