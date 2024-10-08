#define IMGUI_IMPL_VULKAN_USE_VOLK
#include<imgui.h>
#include<imgui_impl_vulkan.h>
#include<imgui_impl_win32.h>

#include<volk.h>

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
		initInfo.PipelineRenderingCreateInfo = vkrhi->getPipelineRenderingCreateInfo();
		initInfo.PipelineCache = vkrhi->_pipelineCache;
        initInfo.ImageCount = vkrhi->getCommandBufferSize();
        initInfo.MinImageCount = vkrhi->getCommandBufferSize();
        initInfo.Allocator = nullptr;
        initInfo.UseDynamicRendering = true;

        ImGui_ImplVulkan_Init(&initInfo);
	}

    void EngineUI::draw(uint32_t index)
    {
        ImDrawData* imDrawData = ImGui::GetDrawData();
        int32_t vertexOffset = 0;
        int32_t indexOffset = 0;

        if ((!imDrawData) || (imDrawData->CmdListsCount == 0)) { return; }

        ImGuiIO& io = ImGui::GetIO();

        auto bindPoint = DRHI::DynamicPipelineBindPoint(_renderer->_rhiContext->getCurrentAPI());
        _renderer->_rhiContext->bindPipeline(_pipeline, bindPoint.PIPELINE_BIND_POINT_GRAPHICS, index);
        _renderer->_rhiContext->bindDescriptorSets(&_descriptorSet, _pipelineLayout, bindPoint.PIPELINE_BIND_POINT_GRAPHICS, index);

        //if api is vulkan
        if (_renderer->_rhiContext->getCurrentAPI() == DRHI::VULKAN)
        {
            VkPipelineLayout pipelineLayout{};
            auto vkrhi = static_cast<DRHI::VulkanDRHI*>(_renderer->_rhiContext.get());
            pushConstBlock.scale = glm::vec2(2.0f / io.DisplaySize.x, 2.0f / io.DisplaySize.y);
            pushConstBlock.translate = glm::vec2(-1.0f);
            vkCmdPushConstants(vkrhi->_commandBuffers[index], pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstBlock), &pushConstBlock);
        }

        _renderer->_rhiContext->bindVertexBuffers(&_vertexBuffer, index);
        _renderer->_rhiContext->bindIndexBuffer(&_indexBuffer, index);

        for (int32_t i = 0; i < imDrawData->CmdListsCount; i++)
        {
            const ImDrawList* cmd_list = imDrawData->CmdLists[i];
            for (int32_t j = 0; j < cmd_list->CmdBuffer.Size; j++)
            {
                const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[j];

                DRHI::DynamicExtent2D extent{};
                extent.width = (uint32_t)(pcmd->ClipRect.z - pcmd->ClipRect.x);
                extent.height = (uint32_t)(pcmd->ClipRect.w - pcmd->ClipRect.y);
                
                DRHI::DynamicOffset2D offset{};
                offset.x = std::max((int32_t)(pcmd->ClipRect.x), 0);
                offset.y = std::max((int32_t)(pcmd->ClipRect.y), 0);

                DRHI::DynamicRect2D rect{};
                rect.extent = extent;
                rect.offset = offset;

                _renderer->_rhiContext->setScissor(index, 0, 1, rect);
                _renderer->_rhiContext->drawIndexed(index, pcmd->ElemCount, 1, indexOffset, vertexOffset, 0);

                indexOffset += pcmd->ElemCount;
            }
            vertexOffset += cmd_list->VtxBuffer.Size;
        }
    }
}