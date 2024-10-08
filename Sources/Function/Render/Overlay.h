#pragma once

#include<Windows.h>
#include<memory>

#include"Renderer.h"

namespace FOCUS
{
	class EngineUI
	{
	private:
		Renderer* _renderer;
		HWND _window;

		DRHI::DynamicPipeline _pipeline{};
		DRHI::DynamicPipelineLayout _pipelineLayout{};
		DRHI::DynamicDescriptorSet _descriptorSet{};

		DRHI::DynamicBuffer _vertexBuffer{};
		DRHI::DynamicBuffer _indexBuffer{};

		float _scale{ 1.0f };
		bool _show_demo_window = true;
		bool _show_another_window = false;
		
		struct PushConstBlock 
		{
			glm::vec2 scale;
			glm::vec2 translate;
		} pushConstBlock;

	public:

		EngineUI() = delete;
		EngineUI(HWND window, Renderer* renderer);

		void initialize();
		void draw(uint32_t index);
		void tick();
	};
}