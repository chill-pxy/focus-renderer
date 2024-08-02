#pragma once
#include <memory>
#include <iostream>

#include "Instance.h"
#include "Device.h"
#include "PhysicalDevice.h"
#include "SwapChain.h"
#include "CommandQueue.h"
#include "Texture.h"
#include "DescriptorHeap.h"

namespace FOCUS
{
	namespace Platform
	{
		enum API
		{
			VULKAN
		};

		class Context
		{
		public:
			Context()
			{
				_runtimeInterface = API::VULKAN;

				_instance = std::make_unique<Instance>();
			}

			Context(API api)
			{
				_runtimeInterface = api;

				_instance = std::make_unique<Instance>();
			}

			void initialize()
			{
				 createInstance(_instance.get());
			}

		private:
			API _runtimeInterface;

			std::unique_ptr<Instance> _instance;
			std::unique_ptr<Device> _device;
			std::unique_ptr<PhysicalDevice> _physicalDevice;
			std::unique_ptr<SwapChain> _swapChain;
			std::unique_ptr<CommandQueue> _commandQueue;
		};
	}
}