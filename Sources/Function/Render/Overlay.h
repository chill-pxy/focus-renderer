#pragma once

#include<Windows.h>
#include<memory>

#include<glm/glm.hpp>

#include"RenderResource.h"

namespace FOCUS
{
	struct PushConstBlock
	{
		glm::vec2 scale;
		glm::vec2 translate;
	};

	class EngineUI : public RenderResource
	{
	private:
		HWND           _window;
		PushConstBlock _pushConstBlock{};

		DRHI::DynamicPipeline       _pipeline;
		DRHI::DynamicPipelineLayout _pipelineLayout;
	
		float          _scale{ 1.0f };
		int            _fontTexWidth{ 0 };
		int            _fontTexHeight{ 0 };
		unsigned char* _fontData{nullptr};
		uint32_t       _fontDataSize{ 0 };

	public:

		EngineUI() = delete;
		EngineUI(HWND window);

		virtual void build(std::shared_ptr<DRHI::DynamicRHI> rhi);
		void initialize();
		void draw(uint32_t index, std::shared_ptr<DRHI::DynamicRHI> rhi);
		void preparePipeline(std::shared_ptr<DRHI::DynamicRHI> rhi);
	};
}