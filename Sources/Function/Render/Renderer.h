#pragma once

#include<memory>
#include<vector>
#include<drhi.h>

#include"RenderPasses\MainPass.h"

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
				DRHI::API::VULKAN,
				rendererCreateInfo.window,
				rendererCreateInfo.extensions
			};

			_rhiContext = std::make_shared<DRHI::Context>(contextCreateinfo);
		}

		void initialize()
		{
			_rhiContext->initialize();
			auto renderpass = new MainPass();
			renderpass->initialize(_rhiContext.get());
		}
	};
}