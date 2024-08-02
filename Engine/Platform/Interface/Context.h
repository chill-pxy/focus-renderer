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
#include "InterfaceType.h"

namespace FOCUS
{
	namespace Platform
	{
		class Context
		{
		public:
			Context()
			{
				//ƒ¨»œ π”√Vulkan
				_runtimeInterface = API::VULKAN;

				_instance = std::make_unique<Instance>(_runtimeInterface);
			}

			Context(API api)
			{
				_runtimeInterface = api;

				_instance = std::make_unique<Instance>(_runtimeInterface);
			}

			void initialize()
			{
				 _instance->createInstance();
				 //_physicalDevice->pickPhysicalDevice(_instance.get());
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