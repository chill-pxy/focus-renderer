#pragma once

#include<memory>
#include<vector>

#include<drhi.h>

#include"Mesh.h"

namespace FOCUS
{
	class Renderer
	{
	public:
		std::shared_ptr<DRHI::DynamicRHI> _rhiContext;

		std::shared_ptr<Mesh> obj;

		DRHI::DynamicPipeline modelPipeline;
		DRHI::DynamicPipelineLayout modelPipelineLayout;

		DRHI::DynamicPipeline uiPipeline;

	public:
		Renderer() = delete;

		Renderer(DRHI::API api, DRHI::PlatformInfo platformCI);
		void updateUniformBuffer(uint32_t currentImage);

		void initialize();
		void buildCommandBuffer();
		void tick();
		void clean();
	};
}