#pragma once

#include<drhi.h>

namespace FOCUS
{
	class RenderPass
	{
	public:
		struct Framebuffer
		{
			int width;
			int height;
			//DRHI::RenderPass* renderPass;
		};

		struct Descriptor
		{
			//DRHI::DescriptorSetLayout descriptorSetLayout;
			//DRHI::DescriptorSet descriptorSet;
		};

	public:
		//virtual void initialize();
		//virtual void draw();
	};
}