#pragma once

namespace FOCUS
{
	namespace Platform
	{
		class Instance
		{
		public:
			void createInstance();
			void createSwapChain();

			void getNumPhysicalDevices();
			void getAllPhysicalDevices();
			void getPhysicalDevice();
		};
	}
}