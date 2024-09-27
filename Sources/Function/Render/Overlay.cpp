#define IMGUI_IMPL_VULKAN_NO_PROTOTYPES
#include<imgui_impl_vulkan.h>
#include<imgui_impl_win32.h>

#include"Overlay.h"

namespace FOCUS
{
    EngineUI::EngineUI(HWND window, Renderer* renderer) :_window(window), _renderer(renderer) {}

	void EngineUI::initialize()
	{
		// Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsLight();

        // Setup Platform/Renderer backends
        ImGui_ImplWin32_Init(_window);
    
        DRHI::DynamicRHI* rhi = _renderer->_rhiContext.get();
        DRHI::VulkanDRHI* vkrhi = static_cast<DRHI::VulkanDRHI*>(rhi);
    
        ImGui_ImplVulkan_InitInfo initInfo{};
        initInfo.Instance = vkrhi->_instance;
        initInfo.PhysicalDevice = vkrhi->_physicalDevice;
        initInfo.DescriptorPool = vkrhi->_descriptorPool;
        initInfo.Device = vkrhi->_device;
        initInfo.Queue = vkrhi->_graphicQueue;
        initInfo.QueueFamily = vkrhi->_queueFamilyIndices.graphicsFamily.value();
        initInfo.ImageCount = vkrhi->getCommandBufferSize();
        initInfo.MinImageCount = vkrhi->getCommandBufferSize();
        initInfo.UseDynamicRendering = true;

        ImGui_ImplVulkan_Init(&initInfo);

	}
}