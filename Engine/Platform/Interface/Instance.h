#pragma once

namespace FOCUS
{
	namespace Platform
	{
		class Instance
		{
		public:
			void createInstance(Instance& instance);
			void createSwapChain();

			void getNumPhysicalDevices();
			void getAllPhysicalDevices();
			void getPhysicalDevice();
		};
	}
}