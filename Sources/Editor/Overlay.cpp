#define IMGUI_IMPL_VULKAN_USE_VOLK
#include<imgui.h>
#include<imgui_impl_vulkan.h>
#include<imgui_impl_win32.h>

#include<volk.h>

#include"Overlay.h"
#include"../Function/Render/RenderSystem.h"
#include"../Platform/WindowSystem.h"

namespace FOCUS
{
    void EngineUI::initialize()
    {
        _rhi = RenderSystem::getInstance()->_renderer->_rhiContext;

        _rhi->createCommandPool(&_commandPool);
        _rhi->createCommandBuffers(&_commandBuffers, &_commandPool);
        _rhi->createViewportImage(&_viewportImages, &_viewportImageMemorys, &_commandPool);
        _rhi->createViewportImageViews(&_viewportImageViews, &_viewportImages);

        // create depth image
        auto api = _rhi->getCurrentAPI();
        auto format = DRHI::DynamicFormat(api);
        auto tilling = DRHI::DynamicImageTiling(api);
        auto useFlag = DRHI::DynamicImageUsageFlagBits(api);
        auto memoryFlag = DRHI::DynamicMemoryPropertyFlags(api);
        auto sampleCount = DRHI::DynamicSampleCountFlags(api);

        _rhi->createDepthStencil(&_viewportDepthImage, &_viewportDepthImageView, &_viewportDepthImageMemory, format.FORMAT_D32_SFLOAT_S8_UINT,_rhi->getSwapChainExtentWidth(), _rhi->getSwapChainExtentHeight(), sampleCount.SAMPLE_COUNT_1_BIT);

        // creata descriptor pool
        {
            DRHI::DynamicDescriptorType type(_rhi->getCurrentAPI());
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

            DRHI::DynamicDescriptorPoolCreateFlag flag(_rhi->getCurrentAPI());
            DRHI::DynamicDescriptorPoolCreateInfo ci{};
            ci.flags = flag.DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
            ci.maxSets = 1000 * poolSizes.size();
            ci.pPoolSizes = &poolSizes;
            _rhi->createDescriptorPool(&_descriptorPool, &ci);
        }

        // create texture sampler
        {
            DRHI::DynamicBorderColor color(_rhi->getCurrentAPI());
            DRHI::DynamicSamplerAddressMode mode(_rhi->getCurrentAPI());
            DRHI::DynamicSamplerCreateInfo sci{};
            sci.borderColor = color.BORDER_COLOR_INT_OPAQUE_BLACK;
            sci.sampleraAddressMode = mode.SAMPLER_ADDRESS_MODE_REPEAT;

            _rhi->createSampler(&_textureSampler, sci);
        }

        // create io
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_IsSRGB;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        setStyle();

        // Setup Platform/Renderer backends
        ImGui_ImplWin32_Init(WindowSystem::getInstance()->getNativeWindow()->_hwnd);

        _backend = _rhi->getCurrentAPI();

        // init for vulkan
        if (_backend == DRHI::VULKAN)
        {
            DRHI::VulkanDRHI* vkrhi = static_cast<DRHI::VulkanDRHI*>(_rhi.get());

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
                _descriptorSets[i].internalID = (VkDescriptorSet)ImGui_ImplVulkan_AddTexture(_textureSampler.getVulkanSampler(), _viewportImageViews[i].getVulkanImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            }
        }

        RenderSystem::getInstance()->_renderer->submitRenderTargetImage(&_viewportImages, &_viewportImageViews, &_viewportDepthImage, &_viewportDepthImageView);

        _prepared = true;
    }

    void EngineUI::draw()
    {
        auto imageLayout = DRHI::DynamicImageLayout(_rhi->getCurrentAPI());
        ImDrawData* imDrawData = ImGui::GetDrawData();
        DRHI::DynamicRenderingInfo renderInfo{};
        renderInfo.isRenderOnSwapChain = true;
        renderInfo.isClearEveryFrame = true;
        renderInfo.depthImageLayout = imageLayout.IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        if (imDrawData != nullptr)
        {
            if (imDrawData->CmdListsCount > 0)
            {
                _isEmpty = false;
                for (uint32_t index = 0; index < _commandBuffers.size(); ++index)
                {
                    renderInfo.swapChainIndex = index;

                    _rhi->beginCommandBuffer(_commandBuffers[index]);
                    _rhi->beginRendering(_commandBuffers[index], renderInfo);

                    if (_backend == DRHI::VULKAN)
                    {
                        ImGui_ImplVulkan_RenderDrawData(imDrawData, _commandBuffers[index].getVulkanCommandBuffer());
                    }

                    _rhi->endRendering(_commandBuffers[index], renderInfo);
                    _rhi->endCommandBuffer(_commandBuffers[index]);
                }

                RenderSystem::getInstance()->_submitCommandBuffers.clear();
                for (uint32_t i = 0; i < _commandBuffers.size(); ++i)
                {
                    RenderSystem::getInstance()->_submitCommandBuffers.push_back(_commandBuffers[i]);
                }

                RenderSystem::getInstance()->_submitCommandBuffers.push_back(
                    RenderSystem::getInstance()->_renderer->_shadowCommandBuffers[RenderSystem::getInstance()->_renderer->_rhiContext->getCurrentFrame()]);

                RenderSystem::getInstance()->_submitCommandBuffers.push_back(
                    RenderSystem::getInstance()->_scene->_sceneCommandBuffers[RenderSystem::getInstance()->_renderer->_rhiContext->getCurrentFrame()]);
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

    void EngineUI::tick(bool* running)
    {
        if (!*running) return;

        if (_backend == DRHI::VULKAN)
        {
            ImGui_ImplVulkan_NewFrame();
        }

        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

        showMenu(running);
        if (!*running) return;

        showSceneUI();
        showPropertyUI();
        showViewPortUI();

        ImGui::Render();

        if(_prepared)
        {
            draw();
        }

        *running = true;
    }

    void EngineUI::recreate()
    {
        ImGui_ImplVulkan_SetMinImageCount(MAX_FRAMES_IN_FLIGHT);

        for (uint32_t i = 0; i < _viewportImages.size(); ++i)
        {
            _rhi->clearImage(&_viewportImageViews[i], &_viewportImages[i], &_viewportImageMemorys[i]);
        }

        _rhi->createViewportImage(&_viewportImages, &_viewportImageMemorys, &_commandPool);
        _rhi->createViewportImageViews(&_viewportImageViews, &_viewportImages);    
        
        _rhi->clearImage(&_viewportDepthImageView, &_viewportDepthImage, &_viewportDepthImageMemory);
        auto api = _rhi->getCurrentAPI();
        auto format = DRHI::DynamicFormat(api);
        auto tilling = DRHI::DynamicImageTiling(api);
        auto useFlag = DRHI::DynamicImageUsageFlagBits(api);
        auto memoryFlag = DRHI::DynamicMemoryPropertyFlags(api);
        auto sampleCount = DRHI::DynamicSampleCountFlags(api);
        _rhi->createDepthStencil(&_viewportDepthImage, &_viewportDepthImageView, &_viewportDepthImageMemory, format.FORMAT_D32_SFLOAT_S8_UINT, _rhi->getSwapChainExtentWidth(), _rhi->getSwapChainExtentHeight(), sampleCount.SAMPLE_COUNT_1_BIT);

        _descriptorSets.resize(_viewportImageViews.size());
        if (_backend == DRHI::VULKAN)
        {
            for (uint32_t i = 0; i < _viewportImageViews.size(); i++)
            {
                _descriptorSets[i].internalID = (VkDescriptorSet)ImGui_ImplVulkan_AddTexture(_textureSampler.getVulkanSampler(), _viewportImageViews[i].getVulkanImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            }
        }

        _prepared = true;
    }

    void EngineUI::clean()
    {
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();

        // clear image
        for (uint32_t i = 0; i < _viewportImages.size(); ++i)
        {
            _rhi->clearImage(&_viewportImageViews[i], &_viewportImages[i], &_viewportImageMemorys[i]);
        }

        _rhi->clearImage(&_viewportDepthImageView, &_viewportDepthImage, &_viewportDepthImageMemory);

        _rhi->clearSampler(&_textureSampler);

        // clear command buffer
        _rhi->freeCommandBuffers(&_commandBuffers, &_commandPool);
        _rhi->destroyCommandPool(&_commandPool);

        // clear descriptor
        for (uint32_t i = 0; i < _descriptorSets.size(); ++i)
        {
            _rhi->freeDescriptorSets(&_descriptorSets[i], &_descriptorPool);
        }
       
        _rhi->clearDescriptorPool(&_descriptorPool);
    } 

    Vector2 EngineUI::getViewportSize()
    {
        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
        return Vector2(viewportPanelSize.x, viewportPanelSize.y);
    }





    //---------------------------- private functions -------------------------------
    void EngineUI::setStyle()
    {
        auto& colors = ImGui::GetStyle().Colors;
        colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.1f, 0.13f, 1.0f };
        colors[ImGuiCol_MenuBarBg] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };

        // Border
        colors[ImGuiCol_Border] = ImVec4{ 0.44f, 0.37f, 0.61f, 0.29f };
        colors[ImGuiCol_BorderShadow] = ImVec4{ 0.0f, 0.0f, 0.0f, 0.24f };

        // Text
        colors[ImGuiCol_Text] = ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f };
        colors[ImGuiCol_TextDisabled] = ImVec4{ 0.5f, 0.5f, 0.5f, 1.0f };

        // Headers
        colors[ImGuiCol_Header] = ImVec4{ 0.13f, 0.13f, 0.17, 1.0f };
        colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.19f, 0.2f, 0.25f, 1.0f };
        colors[ImGuiCol_HeaderActive] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };

        // Buttons
        colors[ImGuiCol_Button] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
        colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.19f, 0.2f, 0.25f, 1.0f };
        colors[ImGuiCol_ButtonActive] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
        colors[ImGuiCol_CheckMark] = ImVec4{ 0.74f, 0.58f, 0.98f, 1.0f };

        // Popups
        colors[ImGuiCol_PopupBg] = ImVec4{ 0.1f, 0.1f, 0.13f, 0.92f };

        // Slider
        colors[ImGuiCol_SliderGrab] = ImVec4{ 0.44f, 0.37f, 0.61f, 0.54f };
        colors[ImGuiCol_SliderGrabActive] = ImVec4{ 0.74f, 0.58f, 0.98f, 0.54f };

        // Frame BG
        colors[ImGuiCol_FrameBg] = ImVec4{ 0.13f, 0.13, 0.17, 1.0f };
        colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.19f, 0.2f, 0.25f, 1.0f };
        colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };

        // Tabs
        colors[ImGuiCol_Tab] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
        colors[ImGuiCol_TabHovered] = ImVec4{ 0.24, 0.24f, 0.32f, 1.0f };
        colors[ImGuiCol_TabActive] = ImVec4{ 0.2f, 0.22f, 0.27f, 1.0f };
        colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };

        // Title
        colors[ImGuiCol_TitleBg] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
        colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };

        // Scrollbar
        colors[ImGuiCol_ScrollbarBg] = ImVec4{ 0.1f, 0.1f, 0.13f, 1.0f };
        colors[ImGuiCol_ScrollbarGrab] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4{ 0.19f, 0.2f, 0.25f, 1.0f };
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4{ 0.24f, 0.24f, 0.32f, 1.0f };

        // Seperator
        colors[ImGuiCol_Separator] = ImVec4{ 0.44f, 0.37f, 0.61f, 1.0f };
        colors[ImGuiCol_SeparatorHovered] = ImVec4{ 0.74f, 0.58f, 0.98f, 1.0f };
        colors[ImGuiCol_SeparatorActive] = ImVec4{ 0.84f, 0.58f, 1.0f, 1.0f };

        // Resize Grip
        colors[ImGuiCol_ResizeGrip] = ImVec4{ 0.44f, 0.37f, 0.61f, 0.29f };
        colors[ImGuiCol_ResizeGripHovered] = ImVec4{ 0.74f, 0.58f, 0.98f, 0.29f };
        colors[ImGuiCol_ResizeGripActive] = ImVec4{ 0.84f, 0.58f, 1.0f, 0.29f };

        // Docking
        colors[ImGuiCol_DockingPreview] = ImVec4{ 0.44f, 0.37f, 0.61f, 1.0f };

        auto& style = ImGui::GetStyle();
        style.TabRounding = 4;
        style.ScrollbarRounding = 9;
        style.WindowRounding = 7;
        style.GrabRounding = 3;
        style.FrameRounding = 3;
        style.PopupRounding = 4;
        style.ChildRounding = 4;
    }

    void EngineUI::showMenu(bool* running)
    {
        // menu bar
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(15, 15));

        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Create"))
                {
                }
                if (ImGui::MenuItem("Open", "Ctrl+O"))
                {
                }
                if (ImGui::MenuItem("Save", "Ctrl+S"))
                {
                }
                if (ImGui::MenuItem("Save as.."))
                {
                }
                ImGui::EndMenu();
            }

            // simulate title bar
            // close window
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12, 12));
            ImGui::SameLine(ImGui::GetWindowSize().x - 50);
            if (ImGui::Button("X"))
            {
                clean();
                *running = false;
                return;
            }

            // resize window
            ImGui::SameLine(ImGui::GetWindowSize().x - 86);
            if (_isMaxSize)
            {
                if (ImGui::Button("o"))
                {
                    WindowSystem::getInstance()->recoverWindow();
                    _isMaxSize = false;
                }
            }
            else
            {
                if (ImGui::Button("O"))
                {
                    WindowSystem::getInstance()->setMaxWindow();
                    _isMaxSize = true;
                }
            }

            // minimal window
            ImGui::SameLine(ImGui::GetWindowSize().x - 122);
            if (ImGui::Button("-"))
            {
                WindowSystem::getInstance()->setMinWindow();
            }
            ImGui::PopStyleVar();

            ImGui::EndMainMenuBar();
        }
        ImGui::PopStyleVar();
    }

    void EngineUI::showSceneUI()
    {
        ImGui::Begin("Scene");

        ImGui::Text("--- World Objects list ---");

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.1f, 0.13f, 1.0f));
        for (auto r : RenderSystem::getInstance()->_scene->_group)
        {
            if (ImGui::Button(r->_name.c_str()))
            {
                _currentObj = r;
            }
        }
        ImGui::PopStyleColor();

        ImGui::Text("--------------------------");

        ImGui::End();
    }

    void EngineUI::showPropertyUI()
    {
        // fps
        ImGui::Begin("Property");
        ImGui::Text("%d fps", RenderSystem::getInstance()->_lastFPS);

        // camera position
        ImGui::Text("camera position x: %f", RenderSystem::getInstance()->_scene->_camera->_position.x);
        ImGui::Text("camera position y: %f", RenderSystem::getInstance()->_scene->_camera->_position.y);
        ImGui::Text("camera position z: %f", RenderSystem::getInstance()->_scene->_camera->_position.z);
        
        // camera rotation
        ImGui::Text("camera rotation x: %f", RenderSystem::getInstance()->_scene->_camera->_rotation.x);
        ImGui::Text("camera rotation y: %f", RenderSystem::getInstance()->_scene->_camera->_rotation.y);
        ImGui::Text("camera rotation z: %f", RenderSystem::getInstance()->_scene->_camera->_rotation.z);

        //light color
        ImGui::DragFloat("point color r", &RenderSystem::getInstance()->_scene->_pointLight->_color.x, 0.1f);
        ImGui::DragFloat("point color g", &RenderSystem::getInstance()->_scene->_pointLight->_color.y, 0.1f);
        ImGui::DragFloat("point color b", &RenderSystem::getInstance()->_scene->_pointLight->_color.z, 0.1f);

        // light strength
        ImGui::DragFloat("point strength", &RenderSystem::getInstance()->_scene->_pointLight->_intensity, 0.1f);

        // dir light
        ImGui::DragFloat("DirLight direction x", &RenderSystem::getInstance()->_scene->_dirLight->_target.x, 0.1f);
        ImGui::DragFloat("DirLight direction y", &RenderSystem::getInstance()->_scene->_dirLight->_target.y, 0.1f);
        ImGui::DragFloat("DirLight direction z", &RenderSystem::getInstance()->_scene->_dirLight->_target.z, 0.1f);

        //dir color
        ImGui::DragFloat("DirLight color r", &RenderSystem::getInstance()->_scene->_dirLight->_color.x, 0.1f);
        ImGui::DragFloat("DirLight color g", &RenderSystem::getInstance()->_scene->_dirLight->_color.y, 0.1f);
        ImGui::DragFloat("DirLight color b", &RenderSystem::getInstance()->_scene->_dirLight->_color.z, 0.1f);

        ImGui::DragFloat("DirLight strength", &RenderSystem::getInstance()->_scene->_dirLight->_intensity, 0.1f);

        // selected obj
        if (_currentObj != nullptr)
        {
            ImGui::Text(_currentObj->_name.c_str());
            ImGui::DragFloat("Position X", &_currentObj->_position.x, 0.1f);
            ImGui::DragFloat("Position Y", &_currentObj->_position.y, 0.1f);
            ImGui::DragFloat("Position Z", &_currentObj->_position.z, 0.1f);

            ImGui::DragFloat("Scale X", &_currentObj->_scale.x, 0.1f);
            ImGui::DragFloat("Scale Y", &_currentObj->_scale.y, 0.1f);
            ImGui::DragFloat("Scale Z", &_currentObj->_scale.z, 0.1f);
        }
       

        ImGui::End();
    }

    void EngineUI::showViewPortUI()
    {
        ImGui::Begin("Viewport");

        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();

        ImGui::Image((ImTextureID)_descriptorSets[_rhi->getCurrentFrame()].getVulkanDescriptorSet(), ImVec2{ viewportPanelSize.x, viewportPanelSize.y });

        if ((_viewportWidth != viewportPanelSize.x) || (_viewportHeight != viewportPanelSize.y))
        {
            _viewportWidth = viewportPanelSize.x;
            _viewportHeight = viewportPanelSize.y;

            RenderSystem::getInstance()->_scene->_canvasWidth = _viewportWidth;
            RenderSystem::getInstance()->_scene->_canvasHeight = _viewportHeight;
        }

        ImGui::End();
    }
}