#pragma once

#include<Windows.h>
#include<memory>

#include<imgui.h>

#include"RenderResource.h"
#include"RenderScene.h"

namespace FOCUS
{
	class EngineUI : public RenderResource
	{
	private:
		HWND           _window;
		uint32_t       _drawCommandCount{ 0 };

		DRHI::API                   _backend{ DRHI::VULKAN };
		DRHI::DynamicDescriptorPool _descriptorPool{};
		 
	public:

		EngineUI() = delete;
		EngineUI(HWND window);

		virtual void build(std::shared_ptr<DRHI::DynamicRHI> rhi) {}
		virtual void draw(uint32_t index, std::shared_ptr<DRHI::DynamicRHI> rhi);
		virtual void updateUniformBuffer(UniformUpdateData uud) {}

		void initialize(std::shared_ptr<DRHI::DynamicRHI> rhi);
		void tick(uint32_t fps, std::shared_ptr<RenderScene> scene);
		bool needUpdate();
	};
}