#define IMGUI_IMPL_VULKAN_USE_VOLK
#include<imgui.h>
#include<imgui_impl_vulkan.h>
#include<imgui_impl_win32.h>

#include<volk.h>

#include"Overlay.h"

namespace FOCUS
{
    EngineUI::EngineUI(HWND window) :_window(window) {}

    void EngineUI::initialize(std::shared_ptr<DRHI::DynamicRHI> rhi)
    {
        rhi->createDescriptorPool(&_descriptorPool);

        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_IsSRGB;

        ImGui::StyleColorsDark();

        // Setup Platform/Renderer backends
        ImGui_ImplWin32_Init(_window);

        _backend = rhi->getCurrentAPI();

        // init for vulkan
        if (_backend == DRHI::VULKAN)
        {

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
    }

    void EngineUI::draw(uint32_t index, std::shared_ptr<DRHI::DynamicRHI> rhi)
    {
        ImDrawData* imDrawData = ImGui::GetDrawData();

        if ((!imDrawData) || (imDrawData->CmdListsCount == 0)) { return; }

        if (_backend == DRHI::VULKAN)
        {
            ImGui_ImplVulkan_RenderDrawData(imDrawData, static_cast<DRHI::VulkanDRHI*>(rhi.get())->_commandBuffers[index]);
        }

        _drawCommandCount = imDrawData->CmdListsCount;
    }

    void EngineUI::tick(uint32_t fps)
    {
        if (_backend == DRHI::VULKAN)
        {
            ImGui_ImplVulkan_NewFrame();
        }

        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        //ImGui::ShowDemoWindow();
        ImGui::Text("%d fps", fps);

        ImGui::Render();
    }

    bool EngineUI::needUpdate()
    {
        ImDrawData* imDrawData = ImGui::GetDrawData();
        if (_drawCommandCount != imDrawData->CmdListsCount)
        {
            return true;
        }

        return false;
    }
}