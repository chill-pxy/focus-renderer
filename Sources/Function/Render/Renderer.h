#pragma once

#include<memory>
#include<vector>
#include<Vulkan/VulkanDRHI.h>

//#include"RenderPasses\MainPass.h"
//
//namespace FOCUS
//{
//	struct RendererCreateInfo
//	{
//		DRHI::API api;
//		DRHI::VulkanGlfwWindowCreateInfo windowCreateInfo;
//	};
//
//	class Renderer
//	{
//	private:
//		DRHI::DynamicRHI* _rhiContext;
//
//	public:
//		Renderer(RendererCreateInfo createInfo)
//		{
//			switch (createInfo.api)
//			{
//			case DRHI::API::VULKAN:
//				DRHI::RHICreatInfo rhiCreateInfo = { createInfo.windowCreateInfo };
//				_rhiContext = new DRHI::VulkanDRHI(rhiCreateInfo);
//				break;
//			}
//		}
//
//		void initialize()
//		{
//			_rhiContext->initialize();
//			auto renderpass = new MainPass();
//			//renderpass->initialize(_rhiContext);
//		}
//
//		GLFWwindow* getRendererWindow()
//		{
//			DRHI::VulkanDRHI* vkRHI = static_cast<DRHI::VulkanDRHI*>(_rhiContext);
//			if (vkRHI)
//			{
//				return vkRHI->_glfwWindow;
//			}
//
//			return nullptr;
//		}
//	};
//}