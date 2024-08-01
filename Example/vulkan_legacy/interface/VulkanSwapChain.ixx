module;
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include <vector>
export module VulkanSwapChain;

import VulkanImageView;

export struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR        capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR>   presentModes;
};

namespace FOCUS
{
	export class VulkanSwapChain
	{
	public:
		VkSwapchainKHR             swapChain;
		std::vector<VkImage>       swapChainImages;
		VkFormat                   swapChainImageFormat;
		VkExtent2D                 swapChainExtent;
		std::vector<VkImageView>   swapChainImageViews;
		std::vector<VkFramebuffer> swapChainFramebuffers;

	public:
		void createSwapChain(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkDevice device, GLFWwindow* window);
		void cleanupSwapChain(VkDevice device, VkImage depthImage, VkImageView depthImageView, VkDeviceMemory depthImageMemory);
		void createImageViews(VkDevice device, VulkanImageView* vkImageView);

		SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
		VkSurfaceFormatKHR      chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR        chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D              chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);
	};
}