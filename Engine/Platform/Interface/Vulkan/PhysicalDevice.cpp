#include "../InterfaceConfig.h"

#ifdef VULKAN_IMPLEMENTATION

#include <stdexcept>
#include <vector>

#include "../PhysicalDevice.h"

namespace FOCUS
{
	namespace Platform
	{
		void PhysicalDevice::pickPhysicalDevice(Instance* instance)
		{
           /* auto vinstance = std::get<VkInstance>(instance->getInstance());
            uint32_t deviceCount = 0;
            vkEnumeratePhysicalDevices(vinstance, &deviceCount, nullptr);

            if (deviceCount == 0) {
                throw std::runtime_error("failed to find GPUs with Vulkan support!");
            }

            std::vector<VkPhysicalDevice> devices(deviceCount);
            vkEnumeratePhysicalDevices(vinstance, &deviceCount, devices.data());*/

           /* for (const auto& device : devices) {
                if (isDeviceSuitable(device)) {
                    this->_runtimePhysicalDevice = device;
                    break;
                }
            }

            if (physicalDevice == VK_NULL_HANDLE) {
                throw std::runtime_error("failed to find a suitable GPU!");
            }*/
		}
	}
}

#endif