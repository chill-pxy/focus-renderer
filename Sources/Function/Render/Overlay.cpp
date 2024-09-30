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

    }

    void EngineUI::tick()
    {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        if (_show_demo_window)
            ImGui::ShowDemoWindow(&_show_demo_window);

        static float f = 0.0f;
        static int counter = 0;
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        ImGuiIO& io = ImGui::GetIO(); (void)io;

        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &_show_demo_window);      // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &_show_another_window);

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();

        if (_show_another_window)
        {
            ImGui::Begin("Another Window", &_show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                _show_another_window = false;
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
    }
}