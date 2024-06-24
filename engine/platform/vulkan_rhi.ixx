module;

#include"vulkan/vulkan.h";
import rhi;

export module vulkan_rhi;

namespace FOCUS
{
	export class VulkanRHI : public RHI
	{
		VulkanRHI();

		virtual void init() override final
		{

		}

		void createInstance()
		{
            VkApplicationInfo app_info{};
            app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            app_info.pApplicationName = "FOCUS Renderer";
            app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
            app_info.pEngineName = "FOCUS";
            app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
            app_info.apiVersion = VK_API_VERSION_1_0;

            VkInstanceCreateInfo create_info{};
            create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            create_info.pApplicationInfo = &app_info;

            //VkResult result = vkCreateInstance(&create_info, nullptr, &instance);
            //if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
            //{
            //    throw std::runtime_error("错误：无法创建实例");
            //}
		}
	};
}