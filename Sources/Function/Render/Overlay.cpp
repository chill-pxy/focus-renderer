#define IMGUI_IMPL_VULKAN_USE_VOLK
#include<imgui.h>
#include<imgui_impl_vulkan.h>
#include<imgui_impl_win32.h>

#include<volk.h>

#include"Overlay.h"

namespace FOCUS
{
    EngineUI::EngineUI(HWND window) :_window(window){}

	void EngineUI::initialize(std::shared_ptr<DRHI::DynamicRHI> rhi)
	{
        //create descriptor
        rhi->createDescriptorPool(&_descriptorPool);

        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        ImGui::StyleColorsDark();

        // Setup Platform/Renderer backends
        ImGui_ImplWin32_Init(_window);

        DRHI::VulkanDRHI* vkrhi = static_cast<DRHI::VulkanDRHI*>(rhi.get());

        ImGui_ImplVulkan_InitInfo initInfo{};
        initInfo.Instance = vkrhi->_instance;
        initInfo.PhysicalDevice = vkrhi->_physicalDevice;
        initInfo.DescriptorPool = _descriptorPool.getVulkanDescriptorPool();
        initInfo.Device = vkrhi->_device;
        initInfo.Queue = vkrhi->_graphicQueue;
        initInfo.QueueFamily = vkrhi->_queueFamilyIndices.graphicsFamily.value();
        initInfo.PipelineRenderingCreateInfo = vkrhi->getPipelineRenderingCreateInfo();
        initInfo.PipelineCache = vkrhi->_pipelineCache;
        initInfo.ImageCount = vkrhi->getCommandBufferSize();
        initInfo.MinImageCount = vkrhi->getCommandBufferSize();
        initInfo.Allocator = nullptr;
        initInfo.UseDynamicRendering = true;

        ImGui_ImplVulkan_Init(&initInfo);
    }

    void EngineUI::draw(uint32_t index, std::shared_ptr<DRHI::DynamicRHI> rhi)
    {
        auto api = rhi->getCurrentAPI();
        auto bindPoint = DRHI::DynamicPipelineBindPoint(api);
        auto stage = DRHI::DynamicShaderStageFlags(api);
        auto indexType = DRHI::DynamicIndexType(api);

        ImDrawData* imDrawData = ImGui::GetDrawData();
        int32_t vertexOffset = 0;
        int32_t indexOffset = 0;

        if ((!imDrawData) || (imDrawData->CmdListsCount == 0)) { return; }

        ImGui_ImplVulkan_RenderDrawData(imDrawData, static_cast<DRHI::VulkanDRHI*>(rhi.get())->_commandBuffers[index]);
    }

    void EngineUI::tick()
    {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ImGui::ShowDemoWindow();
        
        ImGui::Render();
    }
}