module;
#include<vulkan/vulkan.h>

#include<vector>
export module VulkanCommandBuffer;

namespace FOCUS
{
	export class VulkanCommandBuffer
	{
	public:
		VkCommandPool commandPool;
		std::vector<VkCommandBuffer> commandBuffers;

	public:
		void createCommandBuffers();
		void createCommandPool(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkDevice device);
		void endSingleTimeCommands(VkCommandBuffer commandBuffer);
		void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

		VkCommandBuffer beginSingleTimeCommands();
	};
}