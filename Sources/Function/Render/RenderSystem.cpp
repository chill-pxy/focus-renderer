#include<drhi.h>

#include"RenderSystem.h"
#include"RenderScene.h"

#define IMGUI_IMPL_VULKAN_USE_VOLK
#include<imgui.h>
#include<imgui_impl_vulkan.h>
#include<imgui_impl_win32.h>

namespace FOCUS
{
	void RenderSystem::initialize(RenderSystemCreateInfo rsci)
	{
		DRHI::PlatformInfo platformCI{};
		platformCI.window = rsci.window;
		platformCI.width = rsci.width;
		platformCI.height = rsci.height;

		// initialize renderer
		_renderer = std::make_shared<Renderer>(DRHI::VULKAN, platformCI);
		_renderer->initialize();

		// initialize scene
		_scene = std::make_shared<RenderScene>();
		_scene->initialize();

		// initialize ui
		_ui = std::make_shared<EngineUI>(rsci.window);
		_ui->initialize(_renderer->_rhiContext);

		// submit renderable resources
		_renderer->buildAndSubmit(_scene->_group);

		_recreateFunc.push_back(std::bind(&Renderer::buildCommandBuffer, _renderer));
		_recreateFunc.push_back(std::bind_back(&EngineUI::recreate, _ui));
	}

	void RenderSystem::tick()
	{
		auto tStart = std::chrono::high_resolution_clock::now();

		// ui tick
		_ui->tick(_lastFPS, _scene, _renderer->_rhiContext);

		for (int i = 0; i < _renderer->_rhiContext->getCommandBufferSize(); ++i)
		{
			ImDrawData* imDrawData = ImGui::GetDrawData();

			DRHI::VulkanDRHI* vkrhi = static_cast<DRHI::VulkanDRHI*>(_renderer->_rhiContext.get());
			if (imDrawData != nullptr)
			{
				if (imDrawData->CmdListsCount > 0)
				{
					{
						_ui->_isEmpty = false;
						_renderer->_rhiContext->beginCommandBuffer(_ui->_commandBuffers[i]);
						_renderer->_rhiContext->beginInsertMemoryBarrier(i, _ui->_commandBuffers[i]);
						_renderer->_rhiContext->beginRendering(i, _ui->_commandBuffers[i]);
						
						ImGui_ImplVulkan_RenderDrawData(imDrawData, _ui->_commandBuffers[i].getVulkanCommandBuffer());
						_renderer->_rhiContext->endRendering(_ui->_commandBuffers[i]);
						_renderer->_rhiContext->endInsterMemoryBarrier(i, _ui->_commandBuffers[i]);
						_renderer->_rhiContext->endCommandBuffer(_ui->_commandBuffers[i]);
						
					}
				}
				else
				{
					_ui->_isEmpty = true;
				}
			}
			else
			{
				_ui->_isEmpty = true;
			}

			_renderer->_rhiContext->beginCommandBuffer(i);
			_renderer->_rhiContext->beginInsertMemoryBarrier(i);
			_renderer->_rhiContext->beginRendering(i);
			
			for (auto p : _renderer->_submitRenderlist)
			{
				p->draw(i, _renderer->_rhiContext);
			}

			_renderer->_rhiContext->endRendering(i);
			_renderer->_rhiContext->endInsterMemoryBarrier(i);
			_renderer->_rhiContext->endCommandBuffer(i);
			

			//if (imDrawData != nullptr)
			//{
			//	if (imDrawData->CmdListsCount > 0)
			//	{
			//		{
			//			_renderer->_rhiContext->endRendering(_ui->_commandBuffers[i]);
			//			_renderer->_rhiContext->endInsterMemoryBarrier(i, _ui->_commandBuffers[i]);
			//			_renderer->_rhiContext->endCommandBuffer(_ui->_commandBuffers[i]);

			//		}
			//	}
			//}
		}

		// renderer tick
		if (_ui->_isEmpty)
		{
			_renderer->_rhiContext->frameOnTick(_recreateFunc);
		}
		else
		{
			std::vector<DRHI::DynamicCommandBuffer> submitCommandBuffers(1);
			submitCommandBuffers[0] = _ui->_commandBuffers[_renderer->_rhiContext->getCurrentBuffer()];
			_renderer->_rhiContext->frameOnTick(_recreateFunc, submitCommandBuffers);
		}
		//_renderer->buildCommandBuffer();
		

		// compute time on every frame cost
		_frameCounter++;
		
		auto tEnd = std::chrono::high_resolution_clock::now();
		auto tDiff = std::chrono::duration<double, std::milli>(tEnd - tStart).count();
		
		_frameTimer = (float)tDiff / 1000.0f;

		// scene tick
		_scene->tick(_frameTimer);
		
		// Convert to clamped timer value
		_timer += _timerSpeed * _frameTimer;
		if (_timer > 1.0)
		{
			_timer -= 1.0f;
		}

		float fpsTimer = (float)(std::chrono::duration<double, std::milli>(tEnd - _lastTimestamp).count());
		if (fpsTimer > 1000.0f)
		{
			_lastFPS = static_cast<uint32_t>((float)_frameCounter * (1000.0f / fpsTimer));

			_frameCounter = 0;
			_lastTimestamp = tEnd;
		}
		_tPrevEnd = tEnd;
	}

	void RenderSystem::clean()
	{
		_renderer->clean();
	}

	void RenderSystem::setViewportSize(uint32_t width, uint32_t height)
	{
		if (_renderer)
		{
			auto vkrhi = static_cast<DRHI::VulkanDRHI*>(_renderer->_rhiContext.get());
			vkrhi->_viewPortWidth = width;
			vkrhi->_viewPortHeight = height;
		}

		if (_scene)
		{
			_scene->_canvasWidth = width;
			_scene->_canvasHeight = height;
		}
	}

	std::shared_ptr<Renderer> RenderSystem::getRenderer()
	{
		return _renderer;
	}
}