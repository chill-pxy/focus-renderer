module;
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <limits>
#include <optional>
#include <set>
export module vulkan_rhi;




export struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

export struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities = {};
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};



namespace FOCUS
{
    export class VulkanRHI 
    {
    private:
        GLFWwindow*                _window;

        VkInstance                 _instance;
        VkDebugUtilsMessengerEXT   _debugMessenger;
        VkSurfaceKHR               _surface;

        VkPhysicalDevice           _physicalDevice = VK_NULL_HANDLE;
        VkDevice                   _device;

        VkQueue                    _graphicsQueue;
        VkQueue                    _presentQueue;

        VkSwapchainKHR             _swapChain;
        std::vector<VkImage>       _swapChainImages;
        VkFormat                   _swapChainImageFormat;
        VkExtent2D                 _swapChainExtent;
        std::vector<VkImageView>   _swapChainImageViews;
        std::vector<VkFramebuffer> _swapChainFramebuffers;

        VkRenderPass               _renderPass;
        VkPipelineLayout           _pipelineLayout;
        VkPipeline                 _graphicsPipeline;

        VkCommandPool              _commandPool;
        VkCommandBuffer            _commandBuffer;

        VkSemaphore                _imageAvailableSemaphore;
        VkSemaphore                _renderFinishedSemaphore;
        VkFence                    _inFlightFence;

    public:
        void init(GLFWwindow* windows);
        void cleanup();
        void drawTriangle();

    private:
        void createInstance();

        void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

        void setupDebugMessenger();

        void createSurface(GLFWwindow* window);

        void pickPhysicalDevice();

        void createLogicalDevice();

        void createSwapChain();

        void createImageViews();

        void createRenderPass();

        void createGraphicsPipeline();

        void createFramebuffers();

        void createCommandPool();

        void createCommandBuffer();

        void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

        void createSyncObjects();

        VkShaderModule createShaderModule(const std::vector<char>& code);

        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

        bool isDeviceSuitable(VkPhysicalDevice device);

        bool checkDeviceExtensionSupport(VkPhysicalDevice device);

        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

        std::vector<const char*> getRequiredExtensions();

        bool checkValidationLayerSupport();

        static std::vector<char> readFile(const std::string& filename);

        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
    };
}