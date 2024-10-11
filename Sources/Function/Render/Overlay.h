#pragma once

#include<Windows.h>
#include<memory>

#include"Renderer.h"

namespace FOCUS
{
	struct PushConstBlock
	{
		glm::vec2 scale;
		glm::vec2 translate;
	};

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

		PushConstBlock _pushConstBlock{};

		DRHI::DynamicImage _fontImage{};
		DRHI::DynamicDeviceMemory _fontMemory{};
		DRHI::DynamicImageView _fontImageView{};
		DRHI::DynamicBuffer _fontBuffer{};

		float _scale{ 1.0f };

	public:

		EngineUI() = delete;
		EngineUI(HWND window, Renderer* renderer);

		void initialize();
		void draw(uint32_t index);
	};
}