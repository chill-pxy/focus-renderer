#pragma once
#include <memory>

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
		class Context
		{
		public:
			Context() = default;

			void initializeRHI
			(
				Instance& instance, PhysicalDevice& physicalDevice,
				Device& device, CommandQueue& commandQueue,
				SwapChain& swapChain, Texture& backBufferImages, Texture& depthBufferImage,
				DescriptorHeap& rtvHeap, DescriptorHeap& dsvHeap
			);

		private:
			std::unique_ptr<Instance> _instance;
			std::unique_ptr<Device> _device;
			std::unique_ptr<PhysicalDevice> _physicalDevice;
			std::unique_ptr<SwapChain> _swapChain;
			std::unique_ptr<CommandQueue> _commandQueue;
		};
	}
}