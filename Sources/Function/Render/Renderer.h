#pragma once

#include<memory>
#include<vector>

#include<drhi.h>

#include"../../Core/Math.h"
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

	public:
		Renderer() = delete;

		Renderer(DRHI::API api, DRHI::PlatformInfo platformCI);
		
		void updateUniformBuffer(uint32_t currentImage, Matrix4 view);
		void initialize();
		void buildCommandBuffer();
		void tick(Matrix4 view);
		void clean();
	};
}