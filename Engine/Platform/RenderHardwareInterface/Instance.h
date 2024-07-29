#pragma once

namespace RHI
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