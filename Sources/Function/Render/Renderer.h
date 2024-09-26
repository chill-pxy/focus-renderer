#pragma once

#include<memory>
#include<vector>

#include<drhi.h>
#include<imgui.h>

#include"Mesh.h"
#include"Overlay.h"

namespace FOCUS
{
	class Renderer
	{
	private:
		std::shared_ptr<DRHI::DynamicRHI> _rhiContext;

		std::shared_ptr<Mesh> obj;

		std::shared_ptr<EngineUI> ui;

		DRHI::DynamicPipeline modelPipeline;
		DRHI::DynamicPipelineLayout modelPipelineLayout;

		DRHI::DynamicPipeline uiPipeline;

	public:
		Renderer() = delete;

		Renderer(DRHI::API api, DRHI::PlatformInfo platformCI);
		void updateUniformBuffer(uint32_t currentImage);

		void initialize();
		void draw();
		void clean();
	};
}