#pragma once

#include<memory>
#include<vector>

#include<drhi.h>

#include"Mesh.h"
#include"Overlay.h"

namespace FOCUS
{
	class Renderer
	{
	public:
		std::shared_ptr<DRHI::DynamicRHI> _rhiContext;

		std::shared_ptr<Mesh> obj;
		std::shared_ptr<EngineUI> _ui;

		DRHI::DynamicPipeline modelPipeline;
		DRHI::DynamicPipelineLayout modelPipelineLayout;

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