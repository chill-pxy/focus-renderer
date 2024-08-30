#pragma once

#include"RenderPass.h"

namespace FOCUS
{
	class MainPass : public RenderPass
	{
	private:
		DRHI::RenderPass _mainPass;

	public:
		void initialize();
	};
}