module;
#include<vulkan/vulkan.h>

#include<string>
export module VulkanImageView;

import VulkanCommandBuffer;

namespace FOCUS
{
	export class VulkanImageView
	{
	public:
		VkImage depthImage;
		VkDeviceMemory depthImageMemory;
		VkImageView depthImageView;

		VkImage textureImage;
		VkDeviceMemory textureImageMemory;
		VkImageView textureImageView;

	public:
		void createImage
		(
			uint32_t width, 
			uint32_t height, 
			VkFormat format, 
			VkImageTiling tiling, 
			VkImageUsageFlags usage, 
			VkMemoryPropertyFlags properties, 
			VkImage& image, 
			VkDeviceMemory& imageMemory,
			VkDevice device,
			VkPhysicalDevice physicalDevice
		);

		void transitionImageLayout
		(
			VkImage image, 
			VkFormat format, 
			VkImageLayout oldLayout, 
			VkImageLayout newLayout,
			VulkanCommandBuffer* vkCommandBuffer,
			VkDevice device,
			VkQueue graphicsQueue
		);
		
		void copyBufferToImage
		(
			VkBuffer buffer, 
			VkImage image, 
			uint32_t width, 
			uint32_t height,
			VulkanCommandBuffer* vkCommandBuffer,
			VkDevice device,
			VkQueue graphicsQueue
		);

		void createTextureImageView
		(
			VkDevice device
		);

		void createTextureImage
		(
			std::string texturePath,
			VkDevice device,
			VkPhysicalDevice physicalDevice,
			VulkanCommandBuffer* vkCommandBuffer,
			VkQueue graphicsQueue
		);
		
		VkImageView createImageView
		(
			VkImage image, 
			VkFormat format, 
			VkImageAspectFlags aspectFlags,
			VkDevice device
		);
	};
}