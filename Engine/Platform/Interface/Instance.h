#pragma once
#include<variant>

#include<volk.h>

namespace FOCUS
{
	namespace Platform
	{
		class Instance
		{
		public:
			Instance();

			//int为测试类型，实际应设计为其他API的instance类型
			std::variant<VkInstance*, int> _runtimeInstance;

			//void createSwapChain();

			//void getNumPhysicalDevices();
			//void getAllPhysicalDevices();
			//void getPhysicalDevice();
			
		};


		void createInstance(Instance* instance);
	}
}