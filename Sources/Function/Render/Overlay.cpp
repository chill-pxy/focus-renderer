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
        auto imageLayout = DRHI::DynamicImageLayout(api);
        auto descriptorType = DRHI::DynamicDescriptorType(api);
        auto stageFlags = DRHI::DynamicShaderStageFlags(api);
        auto samplerMode = DRHI::DynamicSamplerAddressMode(api);
        auto borderColor = DRHI::DynamicBorderColor(api);

        // create font image and image view
        rhi->createTextureImage(&_textureImage, &_textureMemory, _fontTexWidth, _fontTexHeight, 4, _fontData);
        rhi->createImageView(&_textureImageView, &_textureImage, format.FORMAT_R8G8B8A8_SRGB);

        //create font texture sampler
        DRHI::DynamicSmplerCreateInfo samplerCreateInfo{};
        samplerCreateInfo.sampleraAddressMode = samplerMode.SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerCreateInfo.borderColor = borderColor.BORDER_COLOR_FLOAT_OPAQUE_WHITE;
        rhi->createSampler(&_textureSampler, samplerCreateInfo);

        //create descriptor
        rhi->createDescriptorPool(&_descriptorPool);

        std::vector<DRHI::DynamicDescriptorSetLayoutBinding> dsbs(1);
        dsbs[0].binding = 0;
        dsbs[0].descriptorCount = 1;
        dsbs[0].descriptorType = descriptorType.DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        dsbs[0].pImmutableSamplers = nullptr;
        dsbs[0].stageFlags = stageFlags.SHADER_STAGE_FRAGMENT_BIT;

        rhi->createDescriptorSetLayout(&_descriptorSetLayout, &dsbs);

        DRHI::DynamicDescriptorImageInfo dii{};
        dii.imageLayout = imageLayout.IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        dii.imageView = _textureImageView;
        dii.sampler = _textureSampler;
        
        std::vector<DRHI::DynamicWriteDescriptorSet> wds(1);
        wds[0].descriptorType = descriptorType.DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        wds[0].dstBinding = 0;
        wds[0].pImageInfo = &dii;
        wds[0].descriptorCount = 1;

        rhi->createDescriptorSet(&_descriptorSet, &_descriptorSetLayout, &_descriptorPool, &wds);
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

    void EngineUI::preparePipeline(std::shared_ptr<DRHI::DynamicRHI> rhi)
    {
        auto api = rhi->getCurrentAPI();
        auto stageFlags = DRHI::DynamicShaderStageFlags(api);
        auto format = DRHI::DynamicFormat(api);

        DRHI::DynamicPushConstantRange push{};
        push.offset = 0;
        push.size = sizeof(PushConstBlock);
        push.stageFlags = stageFlags.SHADER_STAGE_VERTEX_BIT;

        DRHI::DynamicPipelineLayoutCreateInfo plci{};
        plci.pSetLayouts = &_descriptorSetLayout;
        plci.setLayoutCount = 1;
        plci.pushConstantRangeCount = 1;
        plci.pPushConstantRanges = &push;

        rhi->createPipelineLayout(&_pipelineLayout, &plci);

        DRHI::PipelineCreateInfo pci{};
        pci.vertexShader = "../../../Shaders/uioverlay.vert.spv";
        pci.fragmentShader = "../../../Shaders/uioverlay.frag.spv";
        pci.vertexInputBinding = DRHI::DynamicVertexInputBindingDescription();
        pci.vertexInputBinding.set(api, 0, sizeof(ImDrawVert));
        pci.vertexInputAttributes = std::vector<DRHI::DynamicVertexInputAttributeDescription>();
        pci.vertexInputAttributes.resize(3);
        pci.vertexInputAttributes[0].set(api, 0, 0, format.FORMAT_R32G32_SFLOAT, offsetof(ImDrawVert, ImDrawVert::pos));
        pci.vertexInputAttributes[1].set(api, 1, 0, format.FORMAT_R32G32_SFLOAT, offsetof(ImDrawVert, ImDrawVert::uv));
        pci.vertexInputAttributes[2].set(api, 2, 0, format.FORMAT_R8G8B8A8_UNORM, offsetof(ImDrawVert, ImDrawVert::col));

        rhi->createPipeline(&_pipeline, &_pipelineLayout, pci);
    }

    void EngineUI::tick()
    {
        ImGuiIO& io = ImGui::GetIO();

        io.DisplaySize = ImVec2((float)1280, (float)720);
        
        ImGui::NewFrame();

        // 设置窗口的初始位置
        ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_FirstUseEver);

        // 开始窗口
        ImGui::Begin("Basic UI Example");

        // 设置每个元素的宽度
        ImGui::PushItemWidth(-ImGui::GetWindowWidth() * 0.5f);

        // 添加一个文本标签
        ImGui::Text("Hello, world!");

        // 添加一个按钮
        if (ImGui::Button("Click Me")) {
            // 按钮点击时执行的操作
        }

        // 添加一个滑动条
        //ImGui::SliderFloat("Slider", &someValue, 0.0f, 1.0f);

        // 撤销宽度设置
        ImGui::PopItemWidth();

        // 结束窗口
        ImGui::End();

        ImGui::Render();
    }

    bool EngineUI::update(std::shared_ptr<DRHI::DynamicRHI> rhi)
    {
        ImDrawData* imDrawData = ImGui::GetDrawData();
        bool updateCmdBuffers = false;

        if (!imDrawData) { return false; };

        // Upload data
        ImDrawVert* vtxDst = new ImDrawVert[imDrawData->TotalVtxCount];
        ImDrawIdx* idxDst = new ImDrawIdx[imDrawData->TotalVtxCount];

        for (int n = 0; n < imDrawData->CmdListsCount; n++)
        {
            const ImDrawList* cmd_list = imDrawData->CmdLists[n];
            memcpy(vtxDst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
            memcpy(idxDst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
            vtxDst += cmd_list->VtxBuffer.Size;
            idxDst += cmd_list->IdxBuffer.Size;
        }

        // Note: Alignment is done inside buffer creation
        uint32_t vertexBufferSize = imDrawData->TotalVtxCount * sizeof(ImDrawVert);
        uint32_t indexBufferSize = imDrawData->TotalVtxCount * sizeof(ImDrawIdx);

        // Update buffers only if vertex or index count has been changed compared to current buffer size
        if ((vertexBufferSize == 0) || (indexBufferSize == 0)) {
            return false;
        }

        auto usage = DRHI::DynamicBufferUsageFlags(rhi->getCurrentAPI());

        // Vertex buffer
        if (( !_vertexBuffer.vaild() ) || (_vertexCount != imDrawData->TotalVtxCount)) 
        {
            rhi->clearBuffer(&_vertexBuffer, &_vertexDeviceMemory);
            rhi->createDynamicBuffer(&_vertexBuffer, &_vertexDeviceMemory, vertexBufferSize, vtxDst, usage.BUFFER_USAGE_VERTEX_BUFFER_BIT);
            _vertexCount = imDrawData->TotalVtxCount;
            rhi->mapMemory(&_vertexDeviceMemory, 0, vertexBufferSize, vtxDst);
            updateCmdBuffers = true;
        }

        // Index buffer
        if ((!_indexBuffer.vaild()) || (_indexCount < imDrawData->TotalIdxCount)) 
        {
            rhi->clearBuffer(&_indexBuffer, &_indexDeviceMemory);
            rhi->createDynamicBuffer(&_indexBuffer, &_indexDeviceMemory, indexBufferSize, idxDst, usage.BUFFER_USAGE_INDEX_BUFFER_BIT);
            _indexCount = imDrawData->TotalIdxCount;
            rhi->mapMemory(&_indexDeviceMemory, 0, indexBufferSize, idxDst);
            updateCmdBuffers = true;
        }

        // Flush to make writes visible to GPU
        //rhi->flushBuffer(&_vertexDeviceMemory, vertexBufferSize, 0);
        //rhi->flushBuffer(& _indexDeviceMemory, indexBufferSize, 0);

        return updateCmdBuffers;
    }
}