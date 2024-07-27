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
		void createCommandBuffers(VkDevice device);
		
		void createCommandPool
		(
			VkPhysicalDevice physicalDevice, 
			VkSurfaceKHR surface, 
			VkDevice device
		);
		
		void endSingleTimeCommands
		(
			VkCommandBuffer commandBuffer, 
			VkQueue graphicsQueue, 
			VkDevice device
		);
		
		void recordCommandBuffer
		(
			VkCommandBuffer commandBuffer,
			uint32_t imageIndex,
			VkRenderPass renderPass,
			VkFramebuffer framebuffer,
			VkExtent2D swapChainExtent,
			VkPipeline graphicsPipeline,
			VkBuffer vertexBuffer,
			VkBuffer indexBuffer,
			VkDescriptorSet descriptorSet,
			VkPipelineLayout pipelineLayout,
			uint32_t size
		);

		void copyBuffer
		(
			VkBuffer srcBuffer, 
			VkBuffer dstBuffer,
			VkDeviceSize size,
			VkDevice device,
			VkQueue graphicsQueue
		);

		VkCommandBuffer beginSingleTimeCommands
		(
			VkDevice device
		);
	};
}