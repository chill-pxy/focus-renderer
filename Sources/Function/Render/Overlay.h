#pragma once

#include<Windows.h>
#include<memory>

#include<glm/glm.hpp>

#include"RenderResource.h"

namespace FOCUS
{
	class EngineUI : public RenderResource
	{
	private:
		HWND           _window;
		uint32_t       _drawCommandCount{ 0 };
		 
	public:

		EngineUI() = delete;
		EngineUI(HWND window);

		virtual void build(std::shared_ptr<DRHI::DynamicRHI> rhi) {}

		void initialize(std::shared_ptr<DRHI::DynamicRHI> rhi);
		void draw(uint32_t index, std::shared_ptr<DRHI::DynamicRHI> rhi);
		void tick();
		bool needUpdate();
	};
}