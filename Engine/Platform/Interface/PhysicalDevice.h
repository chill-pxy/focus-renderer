#pragma once

#include<variant>

#include"volk.h"

#include"Instance.h"

namespace FOCUS
{
	namespace Platform
	{
		class PhysicalDevice
		{
		private:
			std::variant<VkPhysicalDevice*> _runtimePhysicalDevice;

		public:
			void pickPhysicalDevice(Instance* instance);
		};
	}
}