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
        _rhi = rhi;

        rhi->createCommandPool(&_commandPool);
        rhi->createCommandBuffers(&_commandBuffers, &_commandPool);
        //rhi->createViewportImage(&_viewportImages, &_viewportImageMemorys, &_commandPool);
        //rhi->createViewportImageViews(&_viewportImageViews, &_viewportImages);

        // cretea descriptor pool
        {
            DRHI::DynamicDescriptorType type(rhi->getCurrentAPI());
            std::vector<DRHI::DynamicDescriptorPoolSize> poolSizes =
            {
                {type.DESCRIPTOR_TYPE_SAMPLER, 1000},
                {type.DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
                {type.DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
                {type.DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
                {type.DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
                {type.DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
                {type.DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
                {type.DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
                {type.DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
                {type.DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
                {type.DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}
            };

            DRHI::DynamicDescriptorPoolCreateFlag flag(rhi->getCurrentAPI());
            DRHI::DynamicDescriptorPoolCreateInfo ci{};
            ci.flags = flag.DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
            ci.maxSets = 1000 * poolSizes.size();
            ci.pPoolSizes = &poolSizes;
            rhi->createDescriptorPool(&_descriptorPool, &ci);
        }

        // create io
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
            initInfo.ImageCount = MAX_FRAMES_IN_FLIGHT;
            initInfo.MinImageCount = MAX_FRAMES_IN_FLIGHT;
            initInfo.Allocator = nullptr;
            initInfo.UseDynamicRendering = true;

            ImGui_ImplVulkan_Init(&initInfo);

            // create sampler;
            {
                VkSamplerCreateInfo samplerInfo{};
                samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
                samplerInfo.magFilter = VK_FILTER_LINEAR;
                samplerInfo.minFilter = VK_FILTER_LINEAR;
                samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
                samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
                samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
                samplerInfo.anisotropyEnable = VK_FALSE;
                samplerInfo.maxAnisotropy = 1.0f;
                samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
                samplerInfo.unnormalizedCoordinates = VK_FALSE;
                samplerInfo.compareEnable = VK_FALSE;
                samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
                samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
                samplerInfo.mipLodBias = 0.0f;
                samplerInfo.minLod = 0.0f;
                samplerInfo.maxLod = 0.0f;

                if (vkCreateSampler(vkrhi->_device, &samplerInfo, nullptr, &_textureSampler) != VK_SUCCESS)
                {
                    throw std::runtime_error("failed to create texture sampler!");
                }
            }

            //_descriptorSets.resize(_viewportImageViews.size());
            //for (uint32_t i = 0; i < _viewportImageViews.size(); i++)
            //{
            //    _descriptorSets[i] = (VkDescriptorSet)ImGui_ImplVulkan_AddTexture(_textureSampler, _viewportImageViews[i].getVulkanImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            //}
        }

    }

    void EngineUI::draw(std::shared_ptr<DRHI::DynamicRHI> rhi)
    {
        ImDrawData* imDrawData = ImGui::GetDrawData();

        DRHI::VulkanDRHI* vkrhi = static_cast<DRHI::VulkanDRHI*>(rhi.get());
        if (imDrawData != nullptr)
        {
            if (imDrawData->CmdListsCount > 0)
            {
                _isEmpty = false;
                auto index = rhi->getCurrentFrame();

                rhi->beginCommandBuffer(_commandBuffers[index]);
                rhi->beginRendering(_commandBuffers[index], index, false);

                ImGui_ImplVulkan_RenderDrawData(imDrawData, _commandBuffers[index].getVulkanCommandBuffer());

                rhi->endRendering(_commandBuffers[index], index);
                rhi->endCommandBuffer(_commandBuffers[index]);
            }
            else
            {
                _isEmpty = true;
            }
        }
        else
        {
            _isEmpty = true;
        }
    }

    void EngineUI::tick(uint32_t fps, std::shared_ptr<RenderScene> scene, std::shared_ptr<DRHI::DynamicRHI> rhi)
    {
        if (_backend == DRHI::VULKAN)
        {
            ImGui_ImplVulkan_NewFrame();
        }

        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        //ImGui::DockSpaceOverViewport(1, ImGui::GetMainViewport());

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

        DRHI::VulkanDRHI* vkrhi = static_cast<DRHI::VulkanDRHI*>(rhi.get());
        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
        //ImGui::Image((ImTextureID)_descriptorSets[rhi->getCurrentFrame()], ImVec2{viewportPanelSize.x, viewportPanelSize.y});

        ImGui::End();





        ImGui::Render();
        draw(rhi);
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

    void EngineUI::recreate()
    {
        ImGui_ImplVulkan_SetMinImageCount(3);
        //_rhi->freeCommandBuffers(&_commandBuffers, &_commandPool);
        _rhi->createCommandBuffers(&_commandBuffers, &_commandPool);
        //_rhi->createViewportImage(&_viewportImages, &_viewportImageMemorys, &_commandPool);
       // _rhi->createViewportImageViews(&_viewportImageViews, &_viewportImages);
        
       /* _descriptorSets.resize(_viewportImageViews.size());
        for (uint32_t i = 0; i < _viewportImageViews.size(); i++)
        {
            _descriptorSets[i] = (VkDescriptorSet)ImGui_ImplVulkan_AddTexture(_textureSampler, _viewportImageViews[i].getVulkanImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        }*/

        draw(_rhi);
    }
}