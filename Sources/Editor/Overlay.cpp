#define IMGUI_IMPL_VULKAN_USE_VOLK
#include<imgui.h>
#include<imgui_impl_vulkan.h>
#include<imgui_impl_win32.h>

#include<volk.h>

#include"Overlay.h"
#include"IconsMaterialDesign.h"
#include"../Function/Render/RenderSystem.h"
#include"../Platform/WindowSystem.h"

namespace focus
{
    void EngineUI::initialize()
    {
        _rhi = RenderSystemSingleton::getInstance()->_renderer->_rhiContext;

        _rhi->createCommandPool(&_commandPool);
        _rhi->createCommandBuffers(&_commandBuffers, &_commandPool);
        _rhi->createViewportImage(&_viewportImages, &_viewportImageMemorys, &_commandPool);
        _rhi->createViewportImageViews(&_viewportImageViews, &_viewportImages);

        // create depth image
        auto api = _rhi->getCurrentAPI();
        auto format = drhi::DynamicFormat(api);
        auto tilling = drhi::DynamicImageTiling(api);
        auto useFlag = drhi::DynamicImageUsageFlagBits(api);
        auto memoryFlag = drhi::DynamicMemoryPropertyFlags(api);
        auto sampleCount = drhi::DynamicSampleCountFlags(api);

        _rhi->createDepthStencil(&_viewportDepthImage, &_viewportDepthImageView, &_viewportDepthImageMemory, format.FORMAT_D32_SFLOAT_S8_UINT,_rhi->getSwapChainExtentWidth(), _rhi->getSwapChainExtentHeight(), sampleCount.SAMPLE_COUNT_1_BIT);

        // creata descriptor pool
        {
            drhi::DynamicDescriptorType type(_rhi->getCurrentAPI());
            std::vector<drhi::DynamicDescriptorPoolSize> poolSizes =
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

            drhi::DynamicDescriptorPoolCreateFlag flag(_rhi->getCurrentAPI());
            drhi::DynamicDescriptorPoolCreateInfo ci{};
            ci.flags = flag.DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
            ci.maxSets = 1000 * poolSizes.size();
            ci.pPoolSizes = &poolSizes;
            _rhi->createDescriptorPool(&_descriptorPool, &ci);
        }

        // create texture sampler
        {
            drhi::DynamicBorderColor color(_rhi->getCurrentAPI());
            drhi::DynamicSamplerAddressMode mode(_rhi->getCurrentAPI());
            drhi::DynamicSamplerCreateInfo sci{};
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

        // init icon 
        io.Fonts->AddFontDefault();
        ImFontConfig config;
        config.MergeMode = true;
        config.GlyphMinAdvanceX = 32.0f;
        static const ImWchar icon_rangess[] = { ICON_MIN_MD, ICON_MAX_16_MD, 0 };
        io.Fonts->AddFontFromFileTTF(RESOURCE_PATH"Asset/Fonts/MaterialIcons-Regular.ttf", 32.0f, &config, icon_rangess);
        
        setStyle();

        // Setup Platform/Renderer backends
        ImGui_ImplWin32_Init(WindowSystemSingleton::getInstance()->getNativeWindow()->_hwnd);

        _backend = _rhi->getCurrentAPI();

        // init for vulkan
        if (_backend == drhi::VULKAN)
        {
            drhi::VulkanDRHI* vkrhi = static_cast<drhi::VulkanDRHI*>(_rhi.get());

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

        RenderSystemSingleton::getInstance()->_renderer->submitRenderTargetImage(&_viewportImages, &_viewportImageViews, &_viewportDepthImage, &_viewportDepthImageView);

        // init browser
        _browserPath = std::filesystem::path(RESOURCE_PATH"Asset/Models");

        _prepared = true;
    }

    void EngineUI::draw()
    {
        auto imageLayout = drhi::DynamicImageLayout(_rhi->getCurrentAPI());
        ImDrawData* imDrawData = ImGui::GetDrawData();
        drhi::DynamicRenderingInfo renderInfo{};
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

                    if (_backend == drhi::VULKAN)
                    {
                        ImGui_ImplVulkan_RenderDrawData(imDrawData, _commandBuffers[index].getVulkanCommandBuffer());
                    }

                    _rhi->endRendering(_commandBuffers[index], renderInfo);
                    _rhi->endCommandBuffer(_commandBuffers[index]);
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

    void EngineUI::tick(bool* running)
    {
        if (!*running) return;

        if (_backend == drhi::VULKAN)
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
        showInfoUI();
        showFileBrowswerUI();

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
        auto format = drhi::DynamicFormat(api);
        auto tilling = drhi::DynamicImageTiling(api);
        auto useFlag = drhi::DynamicImageUsageFlagBits(api);
        auto memoryFlag = drhi::DynamicMemoryPropertyFlags(api);
        auto sampleCount = drhi::DynamicSampleCountFlags(api);
        _rhi->createDepthStencil(&_viewportDepthImage, &_viewportDepthImageView, &_viewportDepthImageMemory, format.FORMAT_D32_SFLOAT_S8_UINT, _rhi->getSwapChainExtentWidth(), _rhi->getSwapChainExtentHeight(), sampleCount.SAMPLE_COUNT_1_BIT);

        _descriptorSets.resize(_viewportImageViews.size());
        if (_backend == drhi::VULKAN)
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
		style.WindowMenuButtonPosition = ImGuiDir_None;
    }

    void EngineUI::showMenu(bool* running)
    {
        // menu bar
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(15, 20));
        
        if (ImGui::BeginMainMenuBar())
        {
            ImGui::SetCursorScreenPos(ImVec2(15, 10));
            ImGui::SetWindowFontScale(1.0);
            ImGui::Text(ICON_MD_DEVELOPER_BOARD);

            ImGui::SetCursorScreenPos(ImVec2(60, 0));
            if (ImGui::BeginMenu("Start"))
            {
                if (ImGui::MenuItem("Exit"))
                {
                    ImGui::EndMenu();
                    clean();
                    *running = false;
                    return;
                }

                ImGui::EndMenu();
            }

            ImGui::SetCursorScreenPos(ImVec2(120, 0));
            if (ImGui::BeginMenu("Windows"))
            {
                if (ImGui::MenuItem("Scene", "", _sceneOpen))
                {
                    _sceneOpen = !_sceneOpen;
                }

                if (ImGui::MenuItem("Property", "", _propertyOpen))
                {
                    _propertyOpen = !_propertyOpen;
                }

                if (ImGui::MenuItem("ViewPort", "", _viewportOpen))
                {
                    _viewportOpen = !_viewportOpen;
                }

                if (ImGui::MenuItem("Info", "", _infoOpen))
                {
                    _infoOpen = !_infoOpen;
                }

                if (ImGui::MenuItem("FileBrowser", "", _filebrowserOpen))
                {
                    _filebrowserOpen = !_filebrowserOpen;
                }

                ImGui::EndMenu();
            }

            // simulate title bar
            // close window
            ImGui::SetCursorScreenPos(ImVec2(15, 15));
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 15));
            ImGui::SameLine(ImGui::GetWindowSize().x - 50);
            auto iconSize = ImVec2(35, 35);
            ImGui::SetWindowFontScale(0.5);
            if (ImGui::Button(ICON_MD_DISABLED_BY_DEFAULT, iconSize))
            {
                clean();
                *running = false;
                return;
            }

            // resize window
            ImGui::SameLine(ImGui::GetWindowSize().x - 86);
            if (_isMaxSize)
            {
                if (ImGui::Button(ICON_MD_FULLSCREEN_EXIT, iconSize))
                {
                    WindowSystemSingleton::getInstance()->recoverWindow();
                    _isMaxSize = false;
                }
            }
            else
            {
                if (ImGui::Button(ICON_MD_FULLSCREEN, iconSize))
                {
                    WindowSystemSingleton::getInstance()->setMaxWindow();
                    _isMaxSize = true;
                }
            }

            // minimal window
            ImGui::SameLine(ImGui::GetWindowSize().x - 122);
            if (ImGui::Button(ICON_MD_MINIMIZE, iconSize))
            {
                WindowSystemSingleton::getInstance()->setMinWindow();
            }
            ImGui::PopStyleVar();

            ImGui::EndMainMenuBar();
        }
        ImGui::PopStyleVar();
    }

    void EngineUI::showSceneUI()
    {
        if (!_sceneOpen) return;
        ImGui::Begin("Scene");

        ImGui::Columns(2, 0, false);
        ImGui::SetColumnWidth(0, 120);
        ImGui::SetColumnWidth(1, 100);
        ImGui::Text("Object Name");
		ImGui::NextColumn();
        ImGui::Text("Type");
        ImGui::NextColumn();
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.1f, 0.13f, 1.0f));
        for (auto r : RenderSystemSingleton::getInstance()->_scene->_group)
        {
            if (ImGui::Button(r->_name.c_str()))
            {
                _currentObj = r;
            }

            ImGui::NextColumn();
            ImGui::Text("Mesh"); 
            ImGui::NextColumn();
        }
        ImGui::PopStyleColor();

        ImGui::End();
    }

    void EngineUI::showPropertyUI()
    {
        if (!_propertyOpen) return;
        ImGui::Begin("Property");

        // selected obj
        if (_currentObj != nullptr)
        {
            ImGui::TextColored(ImVec4(0, 128, 255, 255), _currentObj->_name.c_str());
            
            // common info
            ImGui::SeparatorText("Common");

            ImGui::Columns(2, 0, false);
            ImGui::SetColumnWidth(0, 70);
            ImGui::SetColumnWidth(1, 300);

            ImGui::Text("Position");
            ImGui::NextColumn();
            float* position[3] = {&_currentObj->_position.x, &_currentObj->_position.y, &_currentObj->_position.z};
            ImGui::DragFloat3("##Position", *position, 0.1);
            ImGui::SameLine();
            ImGui::Text("(Meters)");
            ImGui::NextColumn();

            ImGui::Text("Rotation");
            ImGui::NextColumn();
            float* rotate[3] = { &_currentObj->_rotate.x, &_currentObj->_rotate.y, &_currentObj->_rotate.z };
            ImGui::DragFloat3("##Rotation", *rotate, 0.1);
            ImGui::SameLine();
            ImGui::Text("(Angle)");
            ImGui::NextColumn();

            ImGui::Text("Scale");
            ImGui::NextColumn();
            float* scale[3] = { &_currentObj->_scale.x, &_currentObj->_scale.y, &_currentObj->_scale.z };
            ImGui::DragFloat3("##Scale", *scale, 0.1);
            ImGui::SameLine();
            ImGui::Text("(Percentage)");

            // material info
            ImGui::Columns(1);
            ImGui::SeparatorText("Material");
            ImGui::Text(("Type : " + _currentObj->_material->_type).c_str());
            ImGui::Columns(2, 0, false);
            ImGui::SetColumnWidth(0, 75);
            ImGui::SetColumnWidth(1, 300);

            ImGui::Text("Roughness");
            ImGui::NextColumn();
            ImGui::DragFloat("##MaterialRoughness", &_currentObj->_material->_roughness, 0.01f, 0.0f, 1.0f);
            ImGui::NextColumn();

            ImGui::Text("Metallic");
            ImGui::NextColumn();
            ImGui::DragFloat("##MaterialMetallic", &_currentObj->_material->_metallic, 0.01f, 0.0f, 1.0f);
        }
       
        ImGui::End();
    }

    void EngineUI::showInfoUI()
    {
        if (!_infoOpen) return;
        ImGui::Begin("Infomation");

        ImGui::Text("%d fps", RenderSystemSingleton::getInstance()->_lastFPS);

        auto scene = RenderSystemSingleton::getInstance()->_scene;
        float* pointLightColor[3] = { &scene->_pointLight->_color.x,  &scene->_pointLight->_color.y,  &scene->_pointLight->_color.z };
        ImGui::ColorEdit3("Point Light Color", *pointLightColor);

        // camera position
        ImGui::Text("camera position x: %f", RenderSystemSingleton::getInstance()->_scene->_camera->_position.x);
        ImGui::Text("camera position y: %f", RenderSystemSingleton::getInstance()->_scene->_camera->_position.y);
        ImGui::Text("camera position z: %f", RenderSystemSingleton::getInstance()->_scene->_camera->_position.z);

        // camera rotation
        ImGui::Text("camera rotation x: %f", RenderSystemSingleton::getInstance()->_scene->_camera->_rotation.x);
        ImGui::Text("camera rotation y: %f", RenderSystemSingleton::getInstance()->_scene->_camera->_rotation.y);
        ImGui::Text("camera rotation z: %f", RenderSystemSingleton::getInstance()->_scene->_camera->_rotation.z);

        // light strength
        ImGui::DragFloat("point strength", &RenderSystemSingleton::getInstance()->_scene->_pointLight->_intensity, 0.1f);

        // dir light
        ImGui::DragFloat("DirLight direction x", &RenderSystemSingleton::getInstance()->_scene->_dirLight->_position.x, 0.1f);
        ImGui::DragFloat("DirLight direction y", &RenderSystemSingleton::getInstance()->_scene->_dirLight->_position.y, 0.1f);
        ImGui::DragFloat("DirLight direction z", &RenderSystemSingleton::getInstance()->_scene->_dirLight->_position.z, 0.1f);

        ImGui::DragFloat("DirLight strength", &RenderSystemSingleton::getInstance()->_scene->_dirLight->_intensity, 0.1f);

        ImGui::End();
    }

    void EngineUI::showViewPortUI()
    {
        if (!_viewportOpen) return;
        ImGui::Begin("Viewport");

        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();

        ImGui::Image((ImTextureID)_descriptorSets[_rhi->getCurrentFrame()].getVulkanDescriptorSet(), ImVec2{ viewportPanelSize.x, viewportPanelSize.y });

        if ((_viewportWidth != viewportPanelSize.x) || (_viewportHeight != viewportPanelSize.y))
        {
            _viewportWidth = viewportPanelSize.x;
            _viewportHeight = viewportPanelSize.y;

            RenderSystemSingleton::getInstance()->_scene->_canvasWidth = _viewportWidth;
            RenderSystemSingleton::getInstance()->_scene->_canvasHeight = _viewportHeight;
        }

        ImGui::End();
    }

    //Icon size
    static float padding = 10.0f;
    static float thumbnailSize = 98.0f;

    void EngineUI::showFileBrowswerUI()
    {
        if (!_filebrowserOpen) return;
        ImGui::Begin("Content Browser");

        if (ImGui::Button("<<"))
        {
            _browserPath = _browserPath.parent_path();
            std::cout << _browserPath << std::endl;
        }
        ImGui::SameLine();
        ImGui::Text(_browserPath.string().c_str());

        float cellSize = thumbnailSize + padding;

        float panelWidth = ImGui::GetContentRegionAvail().x;
        int columnCount = (int)(panelWidth / cellSize);
        if (columnCount < 1)
        {
            columnCount = 1;
        }
        ImGui::Columns(columnCount, 0, false);

        for (auto& directory : std::filesystem::directory_iterator(_browserPath))
        {
            ImGui::SetWindowFontScale(1.0);
            const auto& path = directory.path();
            auto relativePath = std::filesystem::relative(path, _browserPath);
            std::string filename = relativePath.filename().string();

            ImGui::PushID(filename.c_str());

            // handle icon with different type
            std::string type = filename;
            if(filename.size() > 4)
                type = filename.substr(filename.size() - 4, 4);

            FileType fileType{};
            auto icon = ICON_MD_DESCRIPTION;
            if (directory.is_directory())
            {
                icon = ICON_MD_FOLDER;
                fileType = FileType::FOLDER;
            }
            else
            {
                if (type == ".obj")
                {
                    icon = ICON_MD_VIEW_IN_AR;
                    fileType = FileType::MODEL;
                }

                if (type == ".jpg" || type == ".png" || type == ".ktx")
                {
                    icon = ICON_MD_PANORAMA;
                    fileType = FileType::IMAGE;
                }
            }

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign , ImVec2(0.5, 0.9));
            // file button
            auto fileButton = ImGui::Button(icon, ImVec2(50, 50));

            // double click event
            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(fileButton))
            {
                if (directory.is_directory())
                    _browserPath = path;
            }

            // drag event
            if (ImGui::IsItemHovered() && ImGui::IsMouseDragging(fileButton))
            {
                _selectedType = fileType;
                _selectedFile = path.string();
            }

            // load obj event
            if (ImGui::IsMouseReleased(fileButton))
            {
                switch (_selectedType)
                {
                case FileType::MODEL:
                {
                    auto scene = RenderSystemSingleton::getInstance()->_scene;
                    auto obj = scene->loadModel(_selectedFile);
                    scene->addModel(obj);
                    RenderSystemSingleton::getInstance()->_renderer->buildAndSubmit(&scene->_group, &scene->_sceneCommandBuffers, &scene->_sceneCommandPool);
                    std::cout << _selectedFile << std::endl;
                    _selectedType = FileType::NONE;
                    break;
                }
                case FileType::FOLDER:
                    _selectedType = FileType::NONE;
                    break;
                case FileType::IMAGE:
                    _selectedType = FileType::NONE;
                    break;
                }
                _selectedFile = "UnSelected";
            }

            ImGui::SetWindowFontScale(0.3);
            ImGui::Text(ICON_MD_BLUR_ON);
            ImGui::SameLine();
            ImGui::SetWindowFontScale(1.0);
            ImGui::Text(filename.c_str());
			ImGui::PopStyleVar();
            ImGui::PopStyleColor();

            ImGui::NextColumn();

            ImGui::PopID();
        }

        ImGui::End();
    }
    
}