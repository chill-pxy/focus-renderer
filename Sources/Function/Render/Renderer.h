#pragma once

#include<memory>
#include<vector>
#include<drhi.h>

namespace FOCUS
{
	struct RendererCreateInfo
	{
		GLFWwindow* window;
		std::vector<const char*> extensions;
	};

	class Renderer
	{
	private:
		std::shared_ptr<DRHI::Context> _rhiContext;

	public:
		Renderer(RendererCreateInfo rendererCreateInfo)
		{
			DRHI::ContextCreatInfo contextCreateinfo = {
				API::VULKAN,
				rendererCreateInfo.window,
				rendererCreateInfo.extensions
			};

			_rhiContext = std::make_shared<DRHI::Context>(contextCreateinfo);
		}

		void initialize()
		{
			_rhiContext->initialize();
		}
	};
}