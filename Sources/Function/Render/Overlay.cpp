#define IMGUI_IMPL_VULKAN_USE_VOLK
#include<imgui.h>
#include<imgui_impl_vulkan.h>
#include<imgui_impl_win32.h>

#include<volk.h>

#include"Overlay.h"

namespace FOCUS
{
    EngineUI::EngineUI(HWND window) :_window(window){}

    void EngineUI::build(std::shared_ptr<DRHI::DynamicRHI> rhi)
    {
        // create target image for copy
        auto api = rhi->getCurrentAPI();
        auto format = DRHI::DynamicFormat(api);
        auto imageTiling = DRHI::DynamicImageTiling(api);
        auto imageUsage = DRHI::DynamicImageUsageFlagBits(api);
        auto memoryFlag = DRHI::DynamicMemoryPropertyFlags(api);
        auto bufferUsage = DRHI::DynamicBufferUsageFlags(api);

        // create font image and image view
       /* _renderer->_rhiContext->createImage(&_fontImage, texWidth, texHeight,
            format.FORMAT_R8G8B8A8_UNORM, imageTiling.IMAGE_TILING_OPTIMAL, imageUsage.IMAGE_USAGE_SAMPLED_BIT | imageUsage.IMAGE_USAGE_TRANSFER_DST_BIT, memoryFlag.MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            &_fontMemory);*/
        rhi->createTextureImage(&_textureImage, &_textureMemory, _fontTexWidth, _fontTexHeight, 4, _fontData);
        rhi->createImageView(&_textureImageView, &_textureImage, format.FORMAT_R8G8B8A8_SRGB);
        //_renderer->_rhiContext->createDynamicBuffer(&_fontBuffer, &_fontMemory, uploadSize, fontData, bufferUsage.BUFFER_USAGE_TRANSFER_SRC_BIT);
        //_renderer->_rhiContext->copyBufferToImage(&_fontBuffer, &_fontImage, texWidth, texHeight);

        //create font texture sampler
        DRHI::DynamicSamplerAddressMode samplerMode(rhi->getCurrentAPI());
        DRHI::DynamicBorderColor borderColor(rhi->getCurrentAPI());
        DRHI::DynamicSmplerCreateInfo samplerCreateInfo{};
        samplerCreateInfo.sampleraAddressMode = samplerMode.SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerCreateInfo.borderColor = borderColor.BORDER_COLOR_FLOAT_OPAQUE_WHITE;
        rhi->createSampler(&_textureSampler, samplerCreateInfo);
    }

	void EngineUI::initialize()
	{
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();

        // create font texture
        const std::string filename = "../../../Asset/Fonts/arial.ttf";
        io.Fonts->AddFontFromFileTTF(filename.c_str(), 16.0f * _scale);

        io.Fonts->GetTexDataAsRGBA32(&_fontData, &_fontTexWidth, &_fontTexHeight);
        _fontDataSize = _fontTexWidth * _fontTexHeight * 4 * sizeof(char);

        //SRS - Set ImGui style scale factor to handle retina and other HiDPI displays (same as font scaling above)
        ImGuiStyle& style = ImGui::GetStyle();
        style.ScaleAllSizes(_scale);
    }

    void EngineUI::draw(uint32_t index, std::shared_ptr<DRHI::DynamicRHI> rhi)
    {
        ImDrawData* imDrawData = ImGui::GetDrawData();
        int32_t vertexOffset = 0;
        int32_t indexOffset = 0;

        if ((!imDrawData) || (imDrawData->CmdListsCount == 0)) { return; }

        ImGuiIO& io = ImGui::GetIO();

        auto bindPoint = DRHI::DynamicPipelineBindPoint(rhi->getCurrentAPI());
        rhi->bindPipeline(_pipeline, bindPoint.PIPELINE_BIND_POINT_GRAPHICS, index);
        rhi->bindDescriptorSets(&_descriptorSet, _pipelineLayout, bindPoint.PIPELINE_BIND_POINT_GRAPHICS, index);

        //if api is vulkan
        if (rhi->getCurrentAPI() == DRHI::VULKAN)
        {
            VkPipelineLayout pipelineLayout{};
            auto vkrhi = static_cast<DRHI::VulkanDRHI*>(rhi.get());
            _pushConstBlock.scale = glm::vec2(2.0f / io.DisplaySize.x, 2.0f / io.DisplaySize.y);
            _pushConstBlock.translate = glm::vec2(-1.0f);
            vkCmdPushConstants(vkrhi->_commandBuffers[index], pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstBlock), &_pushConstBlock);
        }

        rhi->bindVertexBuffers(&_vertexBuffer, index);
        rhi->bindIndexBuffer(&_indexBuffer, index);

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

                rhi->setScissor(index, 0, 1, rect);
                rhi->drawIndexed(index, pcmd->ElemCount, 1, indexOffset, vertexOffset, 0);

                indexOffset += pcmd->ElemCount;
            }
            vertexOffset += cmd_list->VtxBuffer.Size;
        }
    }
}