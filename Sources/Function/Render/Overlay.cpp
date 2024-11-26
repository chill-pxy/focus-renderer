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
        rhi->createViewportImage(&_viewportImages, &_viewportImageMemorys, &_commandPool);
        rhi->createViewportImageViews(&_viewportImageViews, &_viewportImages);

        // creata descriptor pool
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

        // create texture sampler
        {
            DRHI::DynamicBorderColor color(rhi->getCurrentAPI());
            DRHI::DynamicSamplerAddressMode mode(rhi->getCurrentAPI());
            DRHI::DynamicSmplerCreateInfo sci{};
            sci.borderColor = color.BORDER_COLOR_INT_OPAQUE_BLACK;
            sci.sampleraAddressMode = mode.SAMPLER_ADDRESS_MODE_REPEAT;

            rhi->createSampler(&_textureSampler, sci);
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

            _descriptorSets.resize(_viewportImageViews.size());
            for (uint32_t i = 0; i < _viewportImageViews.size(); i++)
            {
                _descriptorSets[i] = (VkDescriptorSet)ImGui_ImplVulkan_AddTexture(_textureSampler.getVulkanSampler(), _viewportImageViews[i].getVulkanImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            }
        }

        _prepared = true;

    }

    void EngineUI::draw(std::shared_ptr<DRHI::DynamicRHI> rhi)
    {
        ImDrawData* imDrawData = ImGui::GetDrawData();
        DRHI::DynamicRenderingInfo renderInfo{};
        renderInfo.isRenderOnSwapChain = true;
        renderInfo.isClearEveryFrame = true;

        if (imDrawData != nullptr)
        {
            if (imDrawData->CmdListsCount > 0)
            {
                _isEmpty = false;
                for (uint32_t index = 0; index < _commandBuffers.size(); ++index)
                {
                    renderInfo.swapChainIndex = index;

                    rhi->beginCommandBuffer(_commandBuffers[index]);
                    rhi->beginRendering(_commandBuffers[index], renderInfo);

                    if (_backend == DRHI::VULKAN)
                    {
                        ImGui_ImplVulkan_RenderDrawData(imDrawData, _commandBuffers[index].getVulkanCommandBuffer());
                    }

                    rhi->endRendering(_commandBuffers[index], renderInfo);
                    rhi->endCommandBuffer(_commandBuffers[index]);
                }
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

        ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

        ImGui::Begin("Property");
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
        
        ImGui::Image((ImTextureID)_descriptorSets[rhi->getCurrentFrame()], ImVec2{viewportPanelSize.x, viewportPanelSize.y});
       
        if ((_viewportWidth != viewportPanelSize.x) || (_viewportHeight != viewportPanelSize.y))
        {
            _viewportWidth = viewportPanelSize.x;
            _viewportHeight = viewportPanelSize.y;

            scene->_canvasWidth = _viewportWidth;
            scene->_canvasHeight = _viewportHeight;
        }

        ImGui::End();





        ImGui::Render();

        if(_prepared)
        {
            draw(rhi);
        }
    }

    void EngineUI::recreate()
    {
        _rhi->createViewportImage(&_viewportImages, &_viewportImageMemorys, &_commandPool);
        _rhi->createViewportImageViews(&_viewportImageViews, &_viewportImages);    
        
        _descriptorSets.resize(_viewportImageViews.size());
        if (_backend == DRHI::VULKAN)
        {
            for (uint32_t i = 0; i < _viewportImageViews.size(); i++)
            {
                _descriptorSets[i] = (VkDescriptorSet)ImGui_ImplVulkan_AddTexture(_textureSampler.getVulkanSampler(), _viewportImageViews[i].getVulkanImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            }
        }

        _prepared = true;

        draw(_rhi);
    }

    void EngineUI::clean()
    {
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();

        for (uint32_t i = 0; i < _viewportImages.size(); ++i)
        {
            _rhi->clearImage(&_viewportImageViews[i], &_viewportImages[i], &_viewportImageMemorys[i]);
        }

        _rhi->clearSampler(&_textureSampler);

        _rhi->freeCommandBuffers(&_commandBuffers, &_commandPool);
        _rhi->destroyCommandPool(&_commandPool);
    }
}