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
	public:
		bool     _updated{ false };

	private:
		HWND           _window;
		PushConstBlock _pushConstBlock{};

		DRHI::DynamicPipeline       _pipeline;
		DRHI::DynamicPipelineLayout _pipelineLayout;
	
		int            _fontTexWidth{ 0 };
		int            _fontTexHeight{ 0 };
		unsigned char* _fontData{nullptr};
		uint32_t       _fontDataSize{ 0 };

		bool     _visible{ true };
		float    _scale{ 1.0f };
		float    _updateTimer{ 0.0f };
		uint32_t _vertexCount{ 0 };
		uint32_t _indexCount{0};

	public:

		EngineUI() = delete;
		EngineUI(HWND window);

		virtual void build(std::shared_ptr<DRHI::DynamicRHI> rhi);
		void initialize(std::shared_ptr<DRHI::DynamicRHI> rhi);
		void draw(uint32_t index, std::shared_ptr<DRHI::DynamicRHI> rhi);
		void preparePipeline(std::shared_ptr<DRHI::DynamicRHI> rhi);
		void tick();
		bool update(std::shared_ptr<DRHI::DynamicRHI> rhi);
	};
}