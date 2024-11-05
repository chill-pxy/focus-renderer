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
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

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

    void EngineUI::tick(uint32_t fps, std::shared_ptr<RenderScene> scene)
    {
        if (_backend == DRHI::VULKAN)
        {
            ImGui_ImplVulkan_NewFrame();
        }

        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        //ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

        ImGui::Begin("Property");
        //ImGui::ShowDemoWindow();
        ImGui::Text("%d fps", fps);

        // light position
        ImGui::DragFloat("point light x", &scene->_light->_position.x, 0.1f);
        ImGui::DragFloat("point light y", &scene->_light->_position.y, 0.1f);
        ImGui::DragFloat("point light z", &scene->_light->_position.z, 0.1f);

        //light color
        ImGui::DragFloat("point color r", &scene->_light->_color.x, 0.1f);
        ImGui::DragFloat("point color g", &scene->_light->_color.y, 0.1f);
        ImGui::DragFloat("point color b", &scene->_light->_color.z, 0.1f);

        // light strength
        ImGui::DragFloat("point strength", &scene->_light->_intensity, 0.1f);

        
        ImGui::End();




        ImGui::Begin("Scene");
        
        // camera position
        ImGui::Text("camera position x: %f", scene->_camera->_position.x);
        ImGui::Text("camera position y: %f", scene->_camera->_position.y);
        ImGui::Text("camera position z: %f", scene->_camera->_position.z);

        
        ImGui::End();





        ImGui::Begin("Viewport");

        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
        //ImGui::Image(_descriptorSets[currentFrame], ImVec2{ viewportPanelSize.x, viewportPanelSize.y });

        ImGui::End();





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