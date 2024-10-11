#define IMGUI_IMPL_VULKAN_USE_VOLK
#include<imgui.h>
#include<imgui_impl_vulkan.h>
#include<imgui_impl_win32.h>

#include<volk.h>

#include"Overlay.h"

namespace FOCUS
{
    EngineUI::EngineUI(HWND window, Renderer* renderer) :_window(window), _renderer(renderer){}

	void EngineUI::initialize()
	{
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();

        // create font texture
        unsigned char* fontData;
        int texWidth, texHeight;

        const std::string filename = "../../../Asset/Fonts/arial.ttf";
        io.Fonts->AddFontFromFileTTF(filename.c_str(), 16.0f * _scale);

        io.Fonts->GetTexDataAsRGBA32(&fontData, &texWidth, &texHeight);
        uint32_t uploadSize = texWidth * texHeight * 4 * sizeof(char);

        //SRS - Set ImGui style scale factor to handle retina and other HiDPI displays (same as font scaling above)
        ImGuiStyle& style = ImGui::GetStyle();
        style.ScaleAllSizes(_scale);

        // create target image for copy
        auto api = _renderer->_rhiContext->getCurrentAPI();
        auto format = DRHI::DynamicFormat(api);
        auto imageTiling = DRHI::DynamicImageTiling(api);
        auto imageUsage = DRHI::DynamicImageUsageFlagBits(api);
        auto memoryFlag = DRHI::DynamicMemoryPropertyFlags(api);
        auto bufferUsage = DRHI::DynamicBufferUsageFlags(api);

        _renderer->_rhiContext->createImage(&_fontImage, texWidth, texHeight,
            format.FORMAT_R8G8B8A8_UNORM, imageTiling.IMAGE_TILING_OPTIMAL, imageUsage.IMAGE_USAGE_SAMPLED_BIT | imageUsage.IMAGE_USAGE_TRANSFER_DST_BIT, memoryFlag.MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            &_fontMemory);

        _renderer->_rhiContext->createImageView(&_fontImageView, &_fontImage, format.FORMAT_R8G8B8A8_UNORM);

        _renderer->_rhiContext->createDynamicBuffer(&_fontBuffer, &_fontMemory, uploadSize, fontData, bufferUsage.BUFFER_USAGE_TRANSFER_SRC_BIT);
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
            _pushConstBlock.scale = glm::vec2(2.0f / io.DisplaySize.x, 2.0f / io.DisplaySize.y);
            _pushConstBlock.translate = glm::vec2(-1.0f);
            vkCmdPushConstants(vkrhi->_commandBuffers[index], pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstBlock), &_pushConstBlock);
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