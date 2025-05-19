#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include<volk.h>

#define NOMINMAX
#include<Windows.h>

#include<variant>
#include<vector>

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

const int MAX_FRAMES_IN_FLIGHT = 3;

namespace drhi
{
	typedef enum API
	{
		VULKAN,
		DIRECT3D12
	}API;

	class DynamicCommandBuffer
	{
	public:
		std::variant<VkCommandBuffer> internalID;

		inline VkCommandBuffer getVulkanCommandBuffer() { return std::get<VkCommandBuffer>(internalID); }

		bool valid()
		{
			if (this)
			{
				if (std::holds_alternative<VkCommandBuffer>(internalID))
				{
					if (std::get<VkCommandBuffer>(internalID) == nullptr) return false;
					else return true;
				}
				else return false;
			}
			else return false;
		}
	};

	class DynamicCommandPool
	{
	public:
		std::variant<VkCommandPool> internalID;

		inline VkCommandPool getVulkanCommandPool() { return std::get<VkCommandPool>(internalID); }

		bool valid()
		{
			if (this)
			{
				if (std::holds_alternative<VkCommandPool>(internalID))
				{
					if (std::get<VkCommandPool>(internalID) == nullptr) return false;
					else return true;
				}
				else return false;
			}
			else return false;
		}
	};

	class DynamicBuffer
	{
	public:
		std::variant<VkBuffer> internalID;

		inline VkBuffer getVulkanBuffer() { return std::get<VkBuffer>(internalID); }

		bool valid()
		{
			if (this)
			{
				if (std::holds_alternative<VkBuffer>(internalID))
				{
					if (std::get<VkBuffer>(internalID) == nullptr) return false;
					else return true;
				}
				else return false;
			}
			else return false;
		}
	};

	class DynamicDeviceMemory
	{
	public:
		std::variant<VkDeviceMemory> internalID;

		inline VkDeviceMemory getVulkanDeviceMemory() { return std::get<VkDeviceMemory>(internalID); }

		bool valid()
		{
			if (this)
			{
				if (std::holds_alternative<VkDeviceMemory>(internalID))
				{
					if (std::get<VkDeviceMemory>(internalID) == nullptr) return false;
					else return true;
				}
				else return false;
			}
			else return false;
		}
	};

	class DynamicImage
	{
	public:
		std::variant<VkImage> internalID;

		inline VkImage getVulkanImage() { return std::get<VkImage>(internalID); }

		bool valid()
		{
			if (this)
			{
				if (std::holds_alternative<VkImage>(internalID))
				{
					if (std::get<VkImage>(internalID) == nullptr) return false;
					else return true;
				}
				else return false;
			}
			else return false;
		}
	};

	class DynamicImageView
	{
	public:
		std::variant<VkImageView> internalID;

		inline VkImageView getVulkanImageView() { return std::get<VkImageView>(internalID); }

		bool valid()
		{
			if (this)
			{
				if (std::holds_alternative<VkImageView>(internalID))
				{
					if (std::get<VkImageView>(internalID) == nullptr) return false;
					else return true;
				}
				else return false;
			}
			else return false;
		}
	};

	class DynamicSampler
	{
	public:
		std::variant<VkSampler> internalID;

		inline VkSampler getVulkanSampler() { return std::get<VkSampler>(internalID); }

		bool valid()
		{
			if (this)
			{
				if (std::holds_alternative<VkSampler>(internalID))
				{
					if (std::get<VkSampler>(internalID) == nullptr) return false;
					else return true;
				}
				else return false;
			}
			else return false;
		}
	};

	class DynamicPipeline
	{
	public:
		std::variant<VkPipeline> internalID;

		inline VkPipeline getVulkanPipeline() { return std::get<VkPipeline>(internalID); }

		bool valid()
		{
			if (this)
			{
				if (std::holds_alternative<VkPipeline>(internalID))
				{
					if (std::get<VkPipeline>(internalID) == nullptr) return false;
					else return true;
				}
				else return false;
			}
			else return false;
		}
	};

	class DynamicPipelineLayout
	{
	public:
		std::variant<VkPipelineLayout> internalID;

		inline VkPipelineLayout getVulkanPipelineLayout() { return std::get<VkPipelineLayout>(internalID); }

		bool valid()
		{
			if (this)
			{
				if (std::holds_alternative<VkPipelineLayout>(internalID))
				{
					if (std::get<VkPipelineLayout>(internalID) == nullptr) return false;
					else return true;
				}
				else return false;
			}
			else return false;
		}
	};

	class DynamicDescriptorPool
	{
	public:
		std::variant<VkDescriptorPool> internalID;

		inline VkDescriptorPool getVulkanDescriptorPool() { return std::get<VkDescriptorPool>(internalID); }

		bool valid()
		{
			if (this)
			{
				if (std::holds_alternative<VkDescriptorPool>(internalID))
				{
					if (std::get<VkDescriptorPool>(internalID) == nullptr) return false;
					else return true;
				}
				else return false;
			}
			else return false;
		}
	};

	class DynamicDescriptorSet
	{
	public:
		std::variant<VkDescriptorSet> internalID;

		inline VkDescriptorSet getVulkanDescriptorSet() { return std::get<VkDescriptorSet>(internalID); }

		bool valid()
		{
			if (this)
			{
				if (std::holds_alternative<VkDescriptorSet>(internalID))
				{
					if (std::get<VkDescriptorSet>(internalID) == nullptr) return false;
					else return true;
				}
				else return false;
			}
			else return false;
		}
	};

	class DynamicDescriptorSetLayout
	{
	public:
		std::variant<VkDescriptorSetLayout> internalID;

		inline VkDescriptorSetLayout getVulkanDescriptorSetLayout() { return std::get<VkDescriptorSetLayout>(internalID); }
	
		bool valid()
		{
			if (this)
			{
				if (std::holds_alternative<VkDescriptorSetLayout>(internalID))
				{
					if (std::get<VkDescriptorSetLayout>(internalID) == nullptr) return false;
					else return true;
				}
				else return false;
			}
			else return false;
		}
	};

	class DynamicRenderPass
	{
	public:
		std::variant<VkRenderPass> internalID;

		inline VkRenderPass getVulkanRenderPass() { return std::get<VkRenderPass>(internalID); }

		bool valid()
		{
			if (this)
			{
				if (std::holds_alternative<VkRenderPass>(internalID))
				{
					if (std::get<VkRenderPass>(internalID) == nullptr) return false;
					else return true;
				}
				else return false;
			}
			else return false;
		}
	};

	class DynamicFramebuffer
	{
	public:
		std::variant<VkFramebuffer> internalID;

		inline VkFramebuffer getVulkanFramebuffer() { return std::get<VkFramebuffer>(internalID); }

		bool valid()
		{
			if (this)
			{
				if (std::holds_alternative<VkFramebuffer>(internalID))
				{
					if (std::get<VkFramebuffer>(internalID) == nullptr) return false;
					else return true;
				}
				else return false;
			}
			else return false;
		}
	};

	class DynamicFilter
	{
	public:
		std::variant<VkFilter> internalID;

		inline VkFilter getVulkanFilter() { return std::get<VkFilter>(internalID); }
	};

	class DynamicDescriptorBufferInfo
	{
	public:
		std::variant<VkDescriptorBufferInfo> internalID;

		void set(API api, DynamicBuffer buffer, uint64_t bufferSize, uint32_t offset = 0)
		{
			switch (api)
			{
			case drhi::VULKAN:
			{
				VkDescriptorBufferInfo vkinfo{};
				vkinfo.buffer = buffer.getVulkanBuffer();
				vkinfo.offset = offset;
				vkinfo.range = bufferSize;

				internalID = vkinfo;
			}break;
			case drhi::DIRECT3D12:
				break;
			}

		}
		inline VkDescriptorBufferInfo getVulkanDescriptorBufferInfo() { return std::get<VkDescriptorBufferInfo>(internalID); }
	};

	class DynamicVertexInputBindingDescription
	{
	public:
		std::variant<VkVertexInputBindingDescription> internalID;

		void set(API api, uint32_t binding, uint32_t stride)
		{
			switch (api)
			{
			case drhi::VULKAN:
			{
				VkVertexInputBindingDescription vkinfo{};
				vkinfo.binding = binding;
				vkinfo.stride = stride;
				vkinfo.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

				internalID = vkinfo;

			}break;
			case drhi::DIRECT3D12:
				break;
			}
		}

		inline VkVertexInputBindingDescription getVulkanVertexInputBindingDescription() { return std::get<VkVertexInputBindingDescription>(internalID); }
	};

	class DynamicVertexInputAttributeDescription
	{
	public:
		std::variant<VkVertexInputAttributeDescription> internalID;

		void set(API api, uint32_t location, uint32_t binding, uint32_t format, uint32_t offset)
		{
			switch (api)
			{
			case drhi::VULKAN:
			{
				VkVertexInputAttributeDescription vkinfo{};
				vkinfo.binding = binding;
				vkinfo.location = location;
				vkinfo.offset = offset;
				vkinfo.format = (VkFormat)format;

				internalID = vkinfo;
			}break;
			case drhi::DIRECT3D12:
				break;
			}
		}

		inline VkVertexInputAttributeDescription getVulkanVertexInputAttributeDescription() { return std::get<VkVertexInputAttributeDescription>(internalID); }
	};

	typedef struct DynamicFormat
	{
		DynamicFormat(API api)
		{
			switch (api)
			{
			case drhi::VULKAN:
				FORMAT_R32G32B32_SFLOAT = VK_FORMAT_R32G32B32_SFLOAT;
				FORMAT_R32G32_SFLOAT = VK_FORMAT_R32G32_SFLOAT;
				FORMAT_R8G8B8A8_UNORM = VK_FORMAT_R8G8B8A8_UNORM;
				FORMAT_R8G8B8A8_SRGB = VK_FORMAT_R8G8B8A8_SRGB;
				FORMAT_B8G8R8A8_SRGB = VK_FORMAT_B8G8R8A8_SRGB;
				FORMAT_D16_UNORM = VK_FORMAT_D16_UNORM;
				FORMAT_D32_SFLOAT_S8_UINT = VK_FORMAT_D32_SFLOAT_S8_UINT;
				FORMAT_UNDEFINED = VK_FORMAT_UNDEFINED;
				FORMAT_R16G16B16A16_SFLOAT = VK_FORMAT_R16G16B16A16_SFLOAT;
				FORMAT_R16G16_SFLOAT = VK_FORMAT_R16G16_SFLOAT;
				FORMAT_R32G32B32A32_SFLOAT = VK_FORMAT_R32G32B32A32_SFLOAT;
				FORMAT_B8G8R8A8_UNORM = VK_FORMAT_B8G8R8A8_UNORM;
				break;
			case drhi::DIRECT3D12:
				break;
			}
		}

		uint32_t
			FORMAT_UNDEFINED{ 0 },
			FORMAT_R32G32B32_SFLOAT{ 0 },
			FORMAT_R32G32_SFLOAT{ 0 },
			FORMAT_R8G8B8A8_UNORM{ 0 },
			FORMAT_R8G8B8A8_SRGB{ 0 },
			FORMAT_B8G8R8A8_SRGB{ 0 },
			FORMAT_D16_UNORM{ 0 },
			FORMAT_D32_SFLOAT_S8_UINT{ 0 },
			FORMAT_R16G16B16A16_SFLOAT{ 0 },
			FORMAT_R16G16_SFLOAT{ 0 },
			FORMAT_R32G32B32A32_SFLOAT{ 0 },
			FORMAT_B8G8R8A8_UNORM{ 0 };

	}DynamicFormat;

	typedef struct DynamicImageTiling
	{
		DynamicImageTiling(API api)
		{
			switch (api)
			{
			case drhi::VULKAN:
				IMAGE_TILING_OPTIMAL = VK_IMAGE_TILING_OPTIMAL;
				IMAGE_TILING_LINEAR = VK_IMAGE_TILING_LINEAR;
				IMAGE_TILING_DRM_FORMAT_MODIFIER_EXT = VK_IMAGE_TILING_DRM_FORMAT_MODIFIER_EXT;
				IMAGE_TILING_MAX_ENUM = VK_IMAGE_TILING_MAX_ENUM;
				break;
			case drhi::DIRECT3D12:
				break;
			}
		}

		uint32_t IMAGE_TILING_OPTIMAL{0};
		uint32_t IMAGE_TILING_LINEAR{0};
		uint32_t IMAGE_TILING_DRM_FORMAT_MODIFIER_EXT{0};
		uint32_t IMAGE_TILING_MAX_ENUM{0};
	}DynamicImageTiling;

	typedef struct DynamicImageUsageFlagBits
	{
		DynamicImageUsageFlagBits(API api)
		{
			switch (api)
			{
			case drhi::VULKAN:
				IMAGE_USAGE_SAMPLED_BIT = VK_IMAGE_USAGE_SAMPLED_BIT;
				IMAGE_USAGE_TRANSFER_DST_BIT = VK_IMAGE_USAGE_TRANSFER_DST_BIT;
				IMAGE_USAGE_TRANSFER_SRC_BIT = VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
				IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
				IMAGE_USAGE_COLOR_ATTACHMENT_BIT = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
				break;
			case drhi::DIRECT3D12:
				break;
			}
		}

		uint32_t
			IMAGE_USAGE_SAMPLED_BIT{ 0 },
			IMAGE_USAGE_TRANSFER_DST_BIT{ 0 },
			IMAGE_USAGE_TRANSFER_SRC_BIT{ 0 },
			IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT{ 0 },
			IMAGE_USAGE_COLOR_ATTACHMENT_BIT{ 0 };
	}DynamicImageUsageFlagBits;

	typedef struct DynamicImageAspectFlagBits
	{
		DynamicImageAspectFlagBits(API api)
		{
			switch (api)
			{
			case drhi::VULKAN:
				IMAGE_ASPECT_COLOR_BIT = VK_IMAGE_ASPECT_COLOR_BIT;
				IMAGE_ASPECT_DEPTH_BIT = VK_IMAGE_ASPECT_DEPTH_BIT;
				IMAGE_ASPECT_STENCIL_BIT = VK_IMAGE_ASPECT_STENCIL_BIT;
				IMAGE_ASPECT_METADATA_BIT = VK_IMAGE_ASPECT_METADATA_BIT;
				IMAGE_ASPECT_PLANE_0_BIT = VK_IMAGE_ASPECT_PLANE_0_BIT;
				IMAGE_ASPECT_PLANE_1_BIT = VK_IMAGE_ASPECT_PLANE_1_BIT;
				IMAGE_ASPECT_PLANE_2_BIT = VK_IMAGE_ASPECT_PLANE_2_BIT;
				IMAGE_ASPECT_NONE = VK_IMAGE_ASPECT_NONE;
				IMAGE_ASPECT_MEMORY_PLANE_0_BIT_EXT = VK_IMAGE_ASPECT_MEMORY_PLANE_0_BIT_EXT;
				IMAGE_ASPECT_MEMORY_PLANE_1_BIT_EXT = VK_IMAGE_ASPECT_MEMORY_PLANE_1_BIT_EXT;
				IMAGE_ASPECT_MEMORY_PLANE_2_BIT_EXT = VK_IMAGE_ASPECT_MEMORY_PLANE_2_BIT_EXT;
				IMAGE_ASPECT_MEMORY_PLANE_3_BIT_EXT = VK_IMAGE_ASPECT_MEMORY_PLANE_3_BIT_EXT;
				IMAGE_ASPECT_PLANE_0_BIT_KHR = VK_IMAGE_ASPECT_PLANE_0_BIT_KHR;
				IMAGE_ASPECT_PLANE_1_BIT_KHR = VK_IMAGE_ASPECT_PLANE_1_BIT_KHR;
				IMAGE_ASPECT_PLANE_2_BIT_KHR = VK_IMAGE_ASPECT_PLANE_2_BIT_KHR;
				IMAGE_ASPECT_NONE_KHR = VK_IMAGE_ASPECT_NONE_KHR;
				IMAGE_ASPECT_FLAG_BITS_MAX_ENUM = VK_IMAGE_ASPECT_FLAG_BITS_MAX_ENUM;
				break;

			case drhi::DIRECT3D12:
				break;
			}
		}

		uint32_t
			IMAGE_ASPECT_COLOR_BIT{ 0 },
			IMAGE_ASPECT_DEPTH_BIT{ 0 },
			IMAGE_ASPECT_STENCIL_BIT{ 0 },
			IMAGE_ASPECT_METADATA_BIT{ 0 },
			IMAGE_ASPECT_PLANE_0_BIT{ 0 },
			IMAGE_ASPECT_PLANE_1_BIT{ 0 },
			IMAGE_ASPECT_PLANE_2_BIT{ 0 },
			IMAGE_ASPECT_NONE{ 0 },
			IMAGE_ASPECT_MEMORY_PLANE_0_BIT_EXT{ 0 },
			IMAGE_ASPECT_MEMORY_PLANE_1_BIT_EXT{ 0 },
			IMAGE_ASPECT_MEMORY_PLANE_2_BIT_EXT{ 0 },
			IMAGE_ASPECT_MEMORY_PLANE_3_BIT_EXT{ 0 },
			IMAGE_ASPECT_PLANE_0_BIT_KHR{ 0 },
			IMAGE_ASPECT_PLANE_1_BIT_KHR{ 0 },
			IMAGE_ASPECT_PLANE_2_BIT_KHR{ 0 },
			IMAGE_ASPECT_NONE_KHR{ 0 },
			IMAGE_ASPECT_FLAG_BITS_MAX_ENUM{ 0 };
	}DynamicImageAspectFlagBits;

	typedef struct DynamicBufferUsageFlags
	{
		DynamicBufferUsageFlags(API api)
		{
			switch (api)
			{
			case drhi::VULKAN:
				BUFFER_USAGE_VERTEX_BUFFER_BIT = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
				BUFFER_USAGE_INDEX_BUFFER_BIT = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
				BUFFER_USAGE_TRANSFER_DST_BIT = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
				BUFFER_USAGE_TRANSFER_SRC_BIT = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
				break;
			case drhi::DIRECT3D12:
				break;
			}
		}

		uint32_t BUFFER_USAGE_VERTEX_BUFFER_BIT{ 0 }, BUFFER_USAGE_INDEX_BUFFER_BIT{ 0 }, BUFFER_USAGE_TRANSFER_DST_BIT{ 0 }, BUFFER_USAGE_TRANSFER_SRC_BIT{ 0 };

	}DynamicBufferUsageFlags;

	typedef struct DynamicPipelineBindPoint
	{
		DynamicPipelineBindPoint(API api)
		{
			switch (api)
			{
			case drhi::VULKAN:
				PIPELINE_BIND_POINT_GRAPHICS = VK_PIPELINE_BIND_POINT_GRAPHICS;
				break;
			case drhi::DIRECT3D12:
				break;
			}
		}

		uint32_t PIPELINE_BIND_POINT_GRAPHICS{ 0 };

	}DynamicPipelineBindPoint;

	typedef struct DynamicSamplerAddressMode
	{
		DynamicSamplerAddressMode(API api)
		{
			switch (api)
			{
			case drhi::VULKAN:
				SAMPLER_ADDRESS_MODE_REPEAT = VK_SAMPLER_ADDRESS_MODE_REPEAT;
				SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
				SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
				SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
				SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE = VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
				SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE_KHR = VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE_KHR;
				SAMPLER_ADDRESS_MODE_MAX_ENUM = VK_SAMPLER_ADDRESS_MODE_MAX_ENUM;
				break;
			case drhi::DIRECT3D12:
				break;
			}
		}

		uint32_t  SAMPLER_ADDRESS_MODE_REPEAT{ 0 }, SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT{ 0 },
			SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE{ 0 }, SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER{ 0 },
			SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE{ 0 },SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE_KHR{ 0 },
			SAMPLER_ADDRESS_MODE_MAX_ENUM{ 0 };

	}DynamicSamplerAddressMode;

	typedef struct DynamicBorderColor
	{
		DynamicBorderColor(API api)
		{
			switch (api)
			{
			case drhi::VULKAN:
				BORDER_COLOR_FLOAT_TRANSPARENT_BLACK = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
				BORDER_COLOR_INT_TRANSPARENT_BLACK = VK_BORDER_COLOR_INT_TRANSPARENT_BLACK;
				BORDER_COLOR_FLOAT_OPAQUE_BLACK = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
				BORDER_COLOR_INT_OPAQUE_BLACK = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
				BORDER_COLOR_FLOAT_OPAQUE_WHITE = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
				BORDER_COLOR_INT_OPAQUE_WHITE = VK_BORDER_COLOR_INT_OPAQUE_WHITE;
				BORDER_COLOR_FLOAT_CUSTOM_EXT = VK_BORDER_COLOR_FLOAT_CUSTOM_EXT;
				BORDER_COLOR_INT_CUSTOM_EXT = VK_BORDER_COLOR_INT_CUSTOM_EXT;
				BORDER_COLOR_MAX_ENUM = VK_BORDER_COLOR_MAX_ENUM;
				break;
			case drhi::DIRECT3D12:
				break;
			}
		}

		uint32_t BORDER_COLOR_FLOAT_TRANSPARENT_BLACK{ 0 },
			BORDER_COLOR_INT_TRANSPARENT_BLACK{ 0 },
			BORDER_COLOR_FLOAT_OPAQUE_BLACK{ 0 },
			BORDER_COLOR_INT_OPAQUE_BLACK{ 0 },
			BORDER_COLOR_FLOAT_OPAQUE_WHITE{ 0 },
			BORDER_COLOR_INT_OPAQUE_WHITE{ 0 },
			BORDER_COLOR_FLOAT_CUSTOM_EXT{ 0 },
			BORDER_COLOR_INT_CUSTOM_EXT{ 0 },
			BORDER_COLOR_MAX_ENUM{ 0 };

	}DynamicBorderColor;

	typedef struct DynamicIndexType
	{
		DynamicIndexType(API api)
		{
			switch (api)
			{
			case VULKAN:
				INDEX_TYPE_UINT16 = VK_INDEX_TYPE_UINT16;
				INDEX_TYPE_UINT32 = VK_INDEX_TYPE_UINT32;
				INDEX_TYPE_NONE_KHR = VK_INDEX_TYPE_NONE_KHR;
				INDEX_TYPE_UINT8_EXT = VK_INDEX_TYPE_UINT8_EXT;
				INDEX_TYPE_NONE_NV = VK_INDEX_TYPE_NONE_NV;
				INDEX_TYPE_MAX_ENUM = VK_INDEX_TYPE_MAX_ENUM;
				break;
			case DIRECT3D12:
				break;
			}
		}

		uint32_t
			INDEX_TYPE_UINT16{ 0 },
			INDEX_TYPE_UINT32{ 0 },
			INDEX_TYPE_NONE_KHR{ 0 },
			INDEX_TYPE_UINT8_EXT{ 0 },
			INDEX_TYPE_NONE_NV{ 0 },
			INDEX_TYPE_MAX_ENUM{ 0 };

	}DynamicIndexType;

	typedef struct DynamicExtent2D
	{
		uint32_t width;
		uint32_t height;
	}DynamicExtent2D;

	typedef struct DynamicOffset2D
	{
		int32_t x;
		int32_t y;
	}DynamicOffset2D;

	typedef struct DynamicRect2D
	{
		DynamicExtent2D extent;
		DynamicOffset2D offset;
	}DynamicRect2D;

	typedef struct DynamicSamplerMipmapMode
	{
		DynamicSamplerMipmapMode(API api)
		{
			switch (api)
			{
			case VULKAN:
				SAMPLER_MIPMAP_MODE_NEAREST = VK_SAMPLER_MIPMAP_MODE_NEAREST;
				SAMPLER_MIPMAP_MODE_LINEAR = VK_SAMPLER_MIPMAP_MODE_LINEAR;
				SAMPLER_MIPMAP_MODE_MAX_ENUM = VK_SAMPLER_MIPMAP_MODE_MAX_ENUM;
				break;
			case DIRECT3D12:
				break;
			}
		}

		uint32_t
			SAMPLER_MIPMAP_MODE_NEAREST{ 0 },
			SAMPLER_MIPMAP_MODE_LINEAR{ 0 },
			SAMPLER_MIPMAP_MODE_MAX_ENUM{ 0 };

	}DynamicSamplerMipmapMode;

	typedef struct DynamicSamplerCreateInfo
	{
		uint32_t sampleraAddressMode;
		uint32_t borderColor;
		uint32_t mipmapMode;
		float minLod;
		float maxLod;
	}DynamicSamplerCreateInfo;

	typedef struct DynamicDescriptorType
	{
		DynamicDescriptorType(API api)
		{
			switch (api)
			{
			case VULKAN:
				DESCRIPTOR_TYPE_SAMPLER = VK_DESCRIPTOR_TYPE_SAMPLER;
				DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				DESCRIPTOR_TYPE_SAMPLED_IMAGE = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
				DESCRIPTOR_TYPE_STORAGE_IMAGE = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
				DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
				DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
				DESCRIPTOR_TYPE_UNIFORM_BUFFER = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				DESCRIPTOR_TYPE_STORAGE_BUFFER = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
				DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
				DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
				DESCRIPTOR_TYPE_INPUT_ATTACHMENT = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
				DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK = VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK;
				DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
				DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV;
				DESCRIPTOR_TYPE_SAMPLE_WEIGHT_IMAGE_QCOM = VK_DESCRIPTOR_TYPE_SAMPLE_WEIGHT_IMAGE_QCOM;
				DESCRIPTOR_TYPE_BLOCK_MATCH_IMAGE_QCOM = VK_DESCRIPTOR_TYPE_BLOCK_MATCH_IMAGE_QCOM;
				DESCRIPTOR_TYPE_MUTABLE_EXT = VK_DESCRIPTOR_TYPE_MUTABLE_EXT;
				DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT = VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT;
				DESCRIPTOR_TYPE_MUTABLE_VALVE = VK_DESCRIPTOR_TYPE_MUTABLE_VALVE;
				DESCRIPTOR_TYPE_MAX_ENUM = VK_DESCRIPTOR_TYPE_MAX_ENUM;
				break;
			case DIRECT3D12:
				break;
			}
		}

		uint32_t 
			DESCRIPTOR_TYPE_SAMPLER{ 0 },
			DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER{ 0 },
			DESCRIPTOR_TYPE_SAMPLED_IMAGE{ 0 },
			DESCRIPTOR_TYPE_STORAGE_IMAGE{ 0 },
			DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER{ 0 },
			DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER{ 0 },
			DESCRIPTOR_TYPE_UNIFORM_BUFFER{ 0 },
			DESCRIPTOR_TYPE_STORAGE_BUFFER{ 0 },
			DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC{ 0 },
			DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC{ 0 },
			DESCRIPTOR_TYPE_INPUT_ATTACHMENT{ 0 },
			DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK{ 0 },
			DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR{ 0 },
			DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV{ 0 },
			DESCRIPTOR_TYPE_SAMPLE_WEIGHT_IMAGE_QCOM{ 0 },
			DESCRIPTOR_TYPE_BLOCK_MATCH_IMAGE_QCOM{ 0 },
			DESCRIPTOR_TYPE_MUTABLE_EXT{ 0 },
			DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT{ 0 },
			DESCRIPTOR_TYPE_MUTABLE_VALVE{ 0 },
			DESCRIPTOR_TYPE_MAX_ENUM{ 0 };
	}DynamicDescriptorType;

	typedef struct DynamicImageLayout 
	{
		DynamicImageLayout(API api)
		{
			switch (api)
			{
			case VULKAN:
				IMAGE_LAYOUT_UNDEFINED = VK_IMAGE_LAYOUT_UNDEFINED;
				IMAGE_LAYOUT_GENERAL = VK_IMAGE_LAYOUT_GENERAL;
				IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
				IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
				IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
				IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				IMAGE_LAYOUT_PREINITIALIZED = VK_IMAGE_LAYOUT_PREINITIALIZED;
				IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL = VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL;
				IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL;
				IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
				IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL = VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL;
				IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL = VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL;
				IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL = VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL;
				IMAGE_LAYOUT_READ_ONLY_OPTIMAL = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL;
				IMAGE_LAYOUT_ATTACHMENT_OPTIMAL = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
				IMAGE_LAYOUT_PRESENT_SRC_KHR = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
				IMAGE_LAYOUT_VIDEO_DECODE_DST_KHR = VK_IMAGE_LAYOUT_VIDEO_DECODE_DST_KHR;
				IMAGE_LAYOUT_VIDEO_DECODE_SRC_KHR = VK_IMAGE_LAYOUT_VIDEO_DECODE_SRC_KHR;
				IMAGE_LAYOUT_VIDEO_DECODE_DPB_KHR = VK_IMAGE_LAYOUT_VIDEO_DECODE_DPB_KHR;
				IMAGE_LAYOUT_SHARED_PRESENT_KHR = VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR;
				IMAGE_LAYOUT_FRAGMENT_DENSITY_MAP_OPTIMAL_EXT = VK_IMAGE_LAYOUT_FRAGMENT_DENSITY_MAP_OPTIMAL_EXT;
				IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR = VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR;
				IMAGE_LAYOUT_ATTACHMENT_FEEDBACK_LOOP_OPTIMAL_EXT = VK_IMAGE_LAYOUT_ATTACHMENT_FEEDBACK_LOOP_OPTIMAL_EXT;
				IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL_KHR = VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL_KHR;
				IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL_KHR = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL_KHR;
				IMAGE_LAYOUT_SHADING_RATE_OPTIMAL_NV = VK_IMAGE_LAYOUT_SHADING_RATE_OPTIMAL_NV;
				IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL_KHR = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL_KHR;
				IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL_KHR = VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL_KHR;
				IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL_KHR = VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL_KHR;
				IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL_KHR = VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL_KHR;
				IMAGE_LAYOUT_READ_ONLY_OPTIMAL_KHR = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL_KHR;
				IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR;
				IMAGE_LAYOUT_MAX_ENUM = VK_IMAGE_LAYOUT_MAX_ENUM;
				break;
			case DIRECT3D12:
				break;
			}
		}

		uint32_t
			IMAGE_LAYOUT_UNDEFINED{0},
			IMAGE_LAYOUT_GENERAL{ 0 },
			IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL{ 0 },
			IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL{ 0 },
			IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL{ 0 },
			IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL{ 0 },
			IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL{ 0 },
			IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL{ 0 },
			IMAGE_LAYOUT_PREINITIALIZED{ 0 },
			IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL{ 0 },
			IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL{ 0 },
			IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL{ 0 },
			IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL{ 0 },
			IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL{ 0 },
			IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL{ 0 },
			IMAGE_LAYOUT_READ_ONLY_OPTIMAL{ 0 },
			IMAGE_LAYOUT_ATTACHMENT_OPTIMAL{ 0 },
			IMAGE_LAYOUT_PRESENT_SRC_KHR{ 0 },
			IMAGE_LAYOUT_VIDEO_DECODE_DST_KHR{ 0 },
			IMAGE_LAYOUT_VIDEO_DECODE_SRC_KHR{ 0 },
			IMAGE_LAYOUT_VIDEO_DECODE_DPB_KHR{ 0 },
			IMAGE_LAYOUT_SHARED_PRESENT_KHR{ 0 },
			IMAGE_LAYOUT_FRAGMENT_DENSITY_MAP_OPTIMAL_EXT{ 0 },
			IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR{ 0 },
			IMAGE_LAYOUT_ATTACHMENT_FEEDBACK_LOOP_OPTIMAL_EXT{ 0 },
			IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL_KHR{ 0 },
			IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL_KHR{ 0 },
			IMAGE_LAYOUT_SHADING_RATE_OPTIMAL_NV{ 0 },
			IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL_KHR{ 0 },
			IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL_KHR{ 0 },
			IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL_KHR{ 0 },
			IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL_KHR{ 0 },
			IMAGE_LAYOUT_READ_ONLY_OPTIMAL_KHR{ 0 },
			IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR{ 0 },
			IMAGE_LAYOUT_MAX_ENUM{ 0 };

	}DynamicImageLayout;

	typedef struct DynamicSampleCountFlags
	{
		DynamicSampleCountFlags(API api)
		{
			switch (api)
			{
			case VULKAN:
				SAMPLE_COUNT_1_BIT = VK_SAMPLE_COUNT_1_BIT;
				SAMPLE_COUNT_2_BIT = VK_SAMPLE_COUNT_2_BIT;
				SAMPLE_COUNT_4_BIT = VK_SAMPLE_COUNT_4_BIT;
				SAMPLE_COUNT_8_BIT = VK_SAMPLE_COUNT_8_BIT;
				SAMPLE_COUNT_16_BIT = VK_SAMPLE_COUNT_16_BIT;
				SAMPLE_COUNT_32_BIT = VK_SAMPLE_COUNT_32_BIT;
				SAMPLE_COUNT_64_BIT = VK_SAMPLE_COUNT_64_BIT;
				SAMPLE_COUNT_FLAG_BITS_MAX_ENUM = VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM;
				break;
			case DIRECT3D12:
				break;
			}
		}

		uint32_t
			SAMPLE_COUNT_1_BIT{ 0 },
			SAMPLE_COUNT_2_BIT{ 0 },
			SAMPLE_COUNT_4_BIT{ 0 },
			SAMPLE_COUNT_8_BIT{ 0 },
			SAMPLE_COUNT_16_BIT{ 0 },
			SAMPLE_COUNT_32_BIT{ 0 },
			SAMPLE_COUNT_64_BIT{ 0 },
			SAMPLE_COUNT_FLAG_BITS_MAX_ENUM{ 0 };

	}DynamicSampleCountFlags;

	typedef struct DynamicShaderStageFlagBits
	{
		DynamicShaderStageFlagBits(API api)
		{
			switch (api)
			{
			case VULKAN:
				SHADER_STAGE_VERTEX_BIT = VK_SHADER_STAGE_VERTEX_BIT;
				SHADER_STAGE_TESSELLATION_CONTROL_BIT = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
				SHADER_STAGE_TESSELLATION_EVALUATION_BIT = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
				SHADER_STAGE_GEOMETRY_BIT = VK_SHADER_STAGE_GEOMETRY_BIT;
				SHADER_STAGE_FRAGMENT_BIT = VK_SHADER_STAGE_FRAGMENT_BIT;
				SHADER_STAGE_COMPUTE_BIT = VK_SHADER_STAGE_COMPUTE_BIT;
				SHADER_STAGE_ALL_GRAPHICS = VK_SHADER_STAGE_ALL_GRAPHICS;
				SHADER_STAGE_ALL = VK_SHADER_STAGE_ALL;
				SHADER_STAGE_RAYGEN_BIT_KHR = VK_SHADER_STAGE_RAYGEN_BIT_KHR;
				SHADER_STAGE_ANY_HIT_BIT_KHR = VK_SHADER_STAGE_ANY_HIT_BIT_KHR;
				SHADER_STAGE_CLOSEST_HIT_BIT_KHR = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
				SHADER_STAGE_MISS_BIT_KHR = VK_SHADER_STAGE_MISS_BIT_KHR;
				SHADER_STAGE_INTERSECTION_BIT_KHR = VK_SHADER_STAGE_INTERSECTION_BIT_KHR;
				SHADER_STAGE_CALLABLE_BIT_KHR = VK_SHADER_STAGE_CALLABLE_BIT_KHR;
				SHADER_STAGE_TASK_BIT_EXT = VK_SHADER_STAGE_TASK_BIT_EXT;
				SHADER_STAGE_MESH_BIT_EXT = VK_SHADER_STAGE_MESH_BIT_EXT;
				SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI = VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI;
				SHADER_STAGE_CLUSTER_CULLING_BIT_HUAWEI = VK_SHADER_STAGE_CLUSTER_CULLING_BIT_HUAWEI;
				SHADER_STAGE_RAYGEN_BIT_NV = VK_SHADER_STAGE_RAYGEN_BIT_NV;
				SHADER_STAGE_ANY_HIT_BIT_NV = VK_SHADER_STAGE_ANY_HIT_BIT_NV;
				SHADER_STAGE_CLOSEST_HIT_BIT_NV = VK_SHADER_STAGE_CLOSEST_HIT_BIT_NV;
				SHADER_STAGE_MISS_BIT_NV = VK_SHADER_STAGE_MISS_BIT_NV;
				SHADER_STAGE_INTERSECTION_BIT_NV = VK_SHADER_STAGE_INTERSECTION_BIT_NV;
				SHADER_STAGE_CALLABLE_BIT_NV = VK_SHADER_STAGE_CALLABLE_BIT_NV;
				SHADER_STAGE_TASK_BIT_NV = VK_SHADER_STAGE_TASK_BIT_NV;
				SHADER_STAGE_MESH_BIT_NV = VK_SHADER_STAGE_MESH_BIT_NV;
				SHADER_STAGE_FLAG_BITS_MAX_ENUM = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
				break;

			case DIRECT3D12:
				break;
			}
		}

		uint32_t
			SHADER_STAGE_VERTEX_BIT{0},
			SHADER_STAGE_TESSELLATION_CONTROL_BIT{ 0 },
			SHADER_STAGE_TESSELLATION_EVALUATION_BIT{ 0 },
			SHADER_STAGE_GEOMETRY_BIT{ 0 },
			SHADER_STAGE_FRAGMENT_BIT{ 0 },
			SHADER_STAGE_COMPUTE_BIT{ 0 },
			SHADER_STAGE_ALL_GRAPHICS{ 0 },
			SHADER_STAGE_ALL{ 0 },
			SHADER_STAGE_RAYGEN_BIT_KHR{ 0 },
			SHADER_STAGE_ANY_HIT_BIT_KHR{ 0 },
			SHADER_STAGE_CLOSEST_HIT_BIT_KHR{ 0 },
			SHADER_STAGE_MISS_BIT_KHR{ 0 },
			SHADER_STAGE_INTERSECTION_BIT_KHR{ 0 },
			SHADER_STAGE_CALLABLE_BIT_KHR{ 0 },
			SHADER_STAGE_TASK_BIT_EXT{ 0 },
			SHADER_STAGE_MESH_BIT_EXT{ 0 },
			SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI{ 0 },
			SHADER_STAGE_CLUSTER_CULLING_BIT_HUAWEI{ 0 },
			SHADER_STAGE_RAYGEN_BIT_NV{ 0 },
			SHADER_STAGE_ANY_HIT_BIT_NV{ 0 },
			SHADER_STAGE_CLOSEST_HIT_BIT_NV{ 0 },
			SHADER_STAGE_MISS_BIT_NV{ 0 },
			SHADER_STAGE_INTERSECTION_BIT_NV{ 0 },
			SHADER_STAGE_CALLABLE_BIT_NV{ 0 },
			SHADER_STAGE_TASK_BIT_NV{ 0 },
			SHADER_STAGE_MESH_BIT_NV{ 0 },
			SHADER_STAGE_FLAG_BITS_MAX_ENUM{ 0 };

	}DynamicShaderStageFlagBits;
	typedef DynamicShaderStageFlagBits DynamicShaderStageFlags;

	typedef struct DynamicMemoryPropertyFlagBits
	{
		DynamicMemoryPropertyFlagBits(API api)
		{
			switch (api)
			{
			case VULKAN:
				MEMORY_PROPERTY_DEVICE_LOCAL_BIT = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
				MEMORY_PROPERTY_HOST_VISIBLE_BIT = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
				MEMORY_PROPERTY_HOST_COHERENT_BIT = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
				MEMORY_PROPERTY_HOST_CACHED_BIT = VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
				MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT = VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT;
				MEMORY_PROPERTY_PROTECTED_BIT = VK_MEMORY_PROPERTY_PROTECTED_BIT;
				MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD = VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD;
				MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD = VK_MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD;
				MEMORY_PROPERTY_RDMA_CAPABLE_BIT_NV = VK_MEMORY_PROPERTY_RDMA_CAPABLE_BIT_NV;
				MEMORY_PROPERTY_FLAG_BITS_MAX_ENUM = VK_MEMORY_PROPERTY_FLAG_BITS_MAX_ENUM;
				break;
			case DIRECT3D12:
				break;
			}
		}

		uint32_t
			MEMORY_PROPERTY_DEVICE_LOCAL_BIT{ 0 },
			MEMORY_PROPERTY_HOST_VISIBLE_BIT{ 0 },
			MEMORY_PROPERTY_HOST_COHERENT_BIT{ 0 },
			MEMORY_PROPERTY_HOST_CACHED_BIT{ 0 },
			MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT{ 0 },
			MEMORY_PROPERTY_PROTECTED_BIT{ 0 },
			MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD{ 0 },
			MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD{ 0 },
			MEMORY_PROPERTY_RDMA_CAPABLE_BIT_NV{ 0 },
			MEMORY_PROPERTY_FLAG_BITS_MAX_ENUM{ 0 };

	}DynamicMemoryPropertyFlagBits;

	typedef struct DynamicCullMode
	{
		DynamicCullMode(API api)
		{
			switch (api)
			{
			case VULKAN:
				CULL_MODE_NONE = VK_CULL_MODE_NONE;
				CULL_MODE_FRONT_BIT = VK_CULL_MODE_FRONT_BIT;
				CULL_MODE_BACK_BIT = VK_CULL_MODE_BACK_BIT;
				CULL_MODE_FRONT_AND_BACK = VK_CULL_MODE_FRONT_AND_BACK;
				CULL_MODE_FLAG_BITS_MAX_ENUM = VK_CULL_MODE_FLAG_BITS_MAX_ENUM;
				break;
			case DIRECT3D12:
				break;
			}
		}

		uint32_t
			CULL_MODE_NONE{ 0 },
			CULL_MODE_FRONT_BIT{ 0 },
			CULL_MODE_BACK_BIT{ 0 },
			CULL_MODE_FRONT_AND_BACK{ 0 },
			CULL_MODE_FLAG_BITS_MAX_ENUM{ 0 };
	};

	typedef  DynamicMemoryPropertyFlagBits DynamicMemoryPropertyFlags;

	typedef struct DynamicExtent3D
	{
		uint32_t    width;
		uint32_t    height;
		uint32_t    depth;
	}DynamicExtent3D;

	typedef struct DynamicImageCreateFlags
	{
		DynamicImageCreateFlags(API api)
		{
			switch (api)
			{
			case VULKAN:
				IMAGE_CREATE_SPARSE_BINDING_BIT = VK_IMAGE_CREATE_SPARSE_BINDING_BIT; 
				IMAGE_CREATE_SPARSE_RESIDENCY_BIT= VK_IMAGE_CREATE_SPARSE_RESIDENCY_BIT; 
				IMAGE_CREATE_SPARSE_ALIASED_BIT= VK_IMAGE_CREATE_SPARSE_ALIASED_BIT; 
				IMAGE_CREATE_MUTABLE_FORMAT_BIT= VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT; 
				IMAGE_CREATE_CUBE_COMPATIBLE_BIT= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT; 
				IMAGE_CREATE_ALIAS_BIT= VK_IMAGE_CREATE_ALIAS_BIT; 
				IMAGE_CREATE_SPLIT_INSTANCE_BIND_REGIONS_BIT= VK_IMAGE_CREATE_SPLIT_INSTANCE_BIND_REGIONS_BIT; 
				IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT= VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT; 
				IMAGE_CREATE_BLOCK_TEXEL_VIEW_COMPATIBLE_BIT= VK_IMAGE_CREATE_BLOCK_TEXEL_VIEW_COMPATIBLE_BIT; 
				IMAGE_CREATE_EXTENDED_USAGE_BIT= VK_IMAGE_CREATE_EXTENDED_USAGE_BIT; 
				IMAGE_CREATE_PROTECTED_BIT= VK_IMAGE_CREATE_PROTECTED_BIT; 
				IMAGE_CREATE_DISJOINT_BIT= VK_IMAGE_CREATE_DISJOINT_BIT; 
				IMAGE_CREATE_CORNER_SAMPLED_BIT_NV= VK_IMAGE_CREATE_CORNER_SAMPLED_BIT_NV; 
				IMAGE_CREATE_SAMPLE_LOCATIONS_COMPATIBLE_DEPTH_BIT_EXT= VK_IMAGE_CREATE_SAMPLE_LOCATIONS_COMPATIBLE_DEPTH_BIT_EXT; 
				IMAGE_CREATE_SUBSAMPLED_BIT_EXT= VK_IMAGE_CREATE_SUBSAMPLED_BIT_EXT; 
				IMAGE_CREATE_DESCRIPTOR_BUFFER_CAPTURE_REPLAY_BIT_EXT= VK_IMAGE_CREATE_DESCRIPTOR_BUFFER_CAPTURE_REPLAY_BIT_EXT; 
				IMAGE_CREATE_MULTISAMPLED_RENDER_TO_SINGLE_SAMPLED_BIT_EXT= VK_IMAGE_CREATE_MULTISAMPLED_RENDER_TO_SINGLE_SAMPLED_BIT_EXT; 
				IMAGE_CREATE_2D_VIEW_COMPATIBLE_BIT_EXT= VK_IMAGE_CREATE_2D_VIEW_COMPATIBLE_BIT_EXT; 
				IMAGE_CREATE_FRAGMENT_DENSITY_MAP_OFFSET_BIT_QCOM= VK_IMAGE_CREATE_FRAGMENT_DENSITY_MAP_OFFSET_BIT_QCOM; 
				IMAGE_CREATE_VIDEO_PROFILE_INDEPENDENT_BIT_KHR= VK_IMAGE_CREATE_VIDEO_PROFILE_INDEPENDENT_BIT_KHR; 
				IMAGE_CREATE_SPLIT_INSTANCE_BIND_REGIONS_BIT_KHR= VK_IMAGE_CREATE_SPLIT_INSTANCE_BIND_REGIONS_BIT_KHR; 
				IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT_KHR= VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT_KHR; 
				IMAGE_CREATE_BLOCK_TEXEL_VIEW_COMPATIBLE_BIT_KHR= VK_IMAGE_CREATE_BLOCK_TEXEL_VIEW_COMPATIBLE_BIT_KHR; 
				IMAGE_CREATE_EXTENDED_USAGE_BIT_KHR= VK_IMAGE_CREATE_EXTENDED_USAGE_BIT_KHR; 
				IMAGE_CREATE_DISJOINT_BIT_KHR= VK_IMAGE_CREATE_DISJOINT_BIT_KHR; 
				IMAGE_CREATE_ALIAS_BIT_KHR = VK_IMAGE_CREATE_ALIAS_BIT_KHR; 
				IMAGE_CREATE_FLAG_BITS_MAX_ENUM = VK_IMAGE_CREATE_FLAG_BITS_MAX_ENUM; 
				break; 
			case DIRECT3D12:
				break; 
			}
		}

		uint32_t
			IMAGE_CREATE_SPARSE_BINDING_BIT{ 0 },
			IMAGE_CREATE_SPARSE_RESIDENCY_BIT{ 0 },
			IMAGE_CREATE_SPARSE_ALIASED_BIT{ 0 },
			IMAGE_CREATE_MUTABLE_FORMAT_BIT{ 0 },
			IMAGE_CREATE_CUBE_COMPATIBLE_BIT{ 0 },
			IMAGE_CREATE_ALIAS_BIT{ 0 },
			IMAGE_CREATE_SPLIT_INSTANCE_BIND_REGIONS_BIT{ 0 },
			IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT{ 0 },
			IMAGE_CREATE_BLOCK_TEXEL_VIEW_COMPATIBLE_BIT{ 0 },
			IMAGE_CREATE_EXTENDED_USAGE_BIT{ 0 },
			IMAGE_CREATE_PROTECTED_BIT{ 0 },
			IMAGE_CREATE_DISJOINT_BIT{ 0 },
			IMAGE_CREATE_CORNER_SAMPLED_BIT_NV{ 0 },
			IMAGE_CREATE_SAMPLE_LOCATIONS_COMPATIBLE_DEPTH_BIT_EXT{ 0 },
			IMAGE_CREATE_SUBSAMPLED_BIT_EXT{ 0 },
			IMAGE_CREATE_DESCRIPTOR_BUFFER_CAPTURE_REPLAY_BIT_EXT{ 0 },
			IMAGE_CREATE_MULTISAMPLED_RENDER_TO_SINGLE_SAMPLED_BIT_EXT{ 0 },
			IMAGE_CREATE_2D_VIEW_COMPATIBLE_BIT_EXT{ 0 },
			IMAGE_CREATE_FRAGMENT_DENSITY_MAP_OFFSET_BIT_QCOM{ 0 },
			IMAGE_CREATE_VIDEO_PROFILE_INDEPENDENT_BIT_KHR{ 0 },
			IMAGE_CREATE_SPLIT_INSTANCE_BIND_REGIONS_BIT_KHR{ 0 },
			IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT_KHR{ 0 },
			IMAGE_CREATE_BLOCK_TEXEL_VIEW_COMPATIBLE_BIT_KHR{ 0 },
			IMAGE_CREATE_EXTENDED_USAGE_BIT_KHR{ 0 },
			IMAGE_CREATE_DISJOINT_BIT_KHR{ 0 },
			IMAGE_CREATE_ALIAS_BIT_KHR{ 0 },
			IMAGE_CREATE_FLAG_BITS_MAX_ENUM{ 0 };
	}DynamicImageCreateFlags;

	typedef struct DynamicImageCreateInfo
	{
		//VkImageCreateFlags       
		uint32_t flags;
		//VkFormat                 
		uint32_t format;
		//VkExtent3D               
		DynamicExtent3D extent;
		uint32_t                 mipLevels;
		uint32_t                 arrayLayers;
		//VkSampleCountFlagBits    
		uint32_t samples;
		//VkImageTiling            
		uint32_t tiling;
		//VkImageUsageFlags        
		uint32_t usage;
		//VkSharingMode            
		uint32_t sharingMode;
		uint32_t                 queueFamilyIndexCount;
		const uint32_t* pQueueFamilyIndices;
		//VkImageLayout
		uint32_t       initialLayout;
	}DynamicImageCreateInfo;

	typedef struct DynamicImageViewCreateFlags
	{
		DynamicImageViewCreateFlags(API api)
		{
			switch (api)
			{
			case VULKAN:
				IMAGE_VIEW_CREATE_FRAGMENT_DENSITY_MAP_DYNAMIC_BIT_EXT = VK_IMAGE_VIEW_CREATE_FRAGMENT_DENSITY_MAP_DYNAMIC_BIT_EXT;
				IMAGE_VIEW_CREATE_DESCRIPTOR_BUFFER_CAPTURE_REPLAY_BIT_EXT = VK_IMAGE_VIEW_CREATE_DESCRIPTOR_BUFFER_CAPTURE_REPLAY_BIT_EXT;
				IMAGE_VIEW_CREATE_FRAGMENT_DENSITY_MAP_DEFERRED_BIT_EXT = VK_IMAGE_VIEW_CREATE_FRAGMENT_DENSITY_MAP_DEFERRED_BIT_EXT;
				IMAGE_VIEW_CREATE_FLAG_BITS_MAX_ENUM = VK_IMAGE_VIEW_CREATE_FLAG_BITS_MAX_ENUM;
				break;
			case DIRECT3D12:
				break;
			}
		}

		uint32_t
			IMAGE_VIEW_CREATE_FRAGMENT_DENSITY_MAP_DYNAMIC_BIT_EXT{ 0 },
			IMAGE_VIEW_CREATE_DESCRIPTOR_BUFFER_CAPTURE_REPLAY_BIT_EXT{ 0 },
			IMAGE_VIEW_CREATE_FRAGMENT_DENSITY_MAP_DEFERRED_BIT_EXT{ 0 },
			IMAGE_VIEW_CREATE_FLAG_BITS_MAX_ENUM{ 0 };
	};

	typedef struct DynamicImageViewType
	{
		DynamicImageViewType(API api)
		{
			switch (api)
			{
			case VULKAN:
				IMAGE_VIEW_TYPE_1D = VK_IMAGE_VIEW_TYPE_1D;
				IMAGE_VIEW_TYPE_2D = VK_IMAGE_VIEW_TYPE_2D;
				IMAGE_VIEW_TYPE_3D = VK_IMAGE_VIEW_TYPE_3D;
				IMAGE_VIEW_TYPE_CUBE = VK_IMAGE_VIEW_TYPE_CUBE;
				IMAGE_VIEW_TYPE_1D_ARRAY = VK_IMAGE_VIEW_TYPE_1D_ARRAY;
				IMAGE_VIEW_TYPE_2D_ARRAY = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
				IMAGE_VIEW_TYPE_CUBE_ARRAY = VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
				IMAGE_VIEW_TYPE_MAX_ENUM = VK_IMAGE_VIEW_TYPE_MAX_ENUM;
				break;
			case DIRECT3D12:
				break;
			}
		}

		uint32_t
			IMAGE_VIEW_TYPE_1D{ 0 },
			IMAGE_VIEW_TYPE_2D{ 0 },
			IMAGE_VIEW_TYPE_3D{ 0 },
			IMAGE_VIEW_TYPE_CUBE{ 0 },
			IMAGE_VIEW_TYPE_1D_ARRAY{ 0 },
			IMAGE_VIEW_TYPE_2D_ARRAY{ 0 },
			IMAGE_VIEW_TYPE_CUBE_ARRAY{ 0 },
			IMAGE_VIEW_TYPE_MAX_ENUM{ 0 };
	}DynamicImageViewType;

	typedef struct DynamicImageSubresourceRange 
	{
		uint32_t    aspectMask;
		uint32_t    baseMipLevel;
		uint32_t    levelCount;
		uint32_t    baseArrayLayer;
		uint32_t    layerCount;
	}DynamicImageSubresourceRange;

	typedef struct DynamicImageViewCreateInfo
	{
		uint32_t                        type;
		DynamicImage                    image;
		//VkFormat                   
		uint32_t                        format;
		DynamicImageSubresourceRange    subresourceRange;
	}DynamicImageViewCreateInfo;

	typedef struct DynamicDescriptorImageInfo {
		DynamicSampler        sampler;
		DynamicImageView      imageView;
		uint32_t              imageLayout;
	} DynamicDescriptorImageInfo;

	typedef struct DynamicWriteDescriptorSet
	{
		uint32_t                           dstBinding;
		uint32_t                           dstArrayElement;
		uint32_t                           descriptorCount;
		uint32_t                           descriptorType;
		DynamicDescriptorImageInfo*        pImageInfo;
		DynamicDescriptorBufferInfo*       pBufferInfo;
	}DynamicWriteDescriptorSet;

	typedef struct DynamicDescriptorSetLayoutBinding
	{
		uint32_t          binding;
		uint32_t          descriptorType;
		uint32_t          descriptorCount;
		uint32_t          stageFlags;
		DynamicSampler*   pImmutableSamplers;
	}DynamicDescriptorSetLayoutBinding;

	typedef struct DynamicPushConstantRange
	{
		uint32_t              stageFlags;
		uint32_t              offset;
		uint32_t              size;
	}DynamicPushConstantRange;

	typedef struct DynamicPipelineLayoutCreateInfo
	{
		const void*                     pNext;
		uint32_t                        setLayoutCount;
		DynamicDescriptorSetLayout*     pSetLayouts;
		uint32_t                        pushConstantRangeCount;
		DynamicPushConstantRange*       pPushConstantRanges;
	}DynamicPipelineLayoutCreateInfo;

	typedef struct DynamicPipelineCreateInfo
	{
		const char* vertexShader;
		const char* fragmentShader;
		DynamicVertexInputBindingDescription vertexInputBinding;
		std::vector<DynamicVertexInputAttributeDescription> vertexInputAttributes;
		uint32_t colorImageFormat;
		uint32_t depthImageFormat;
		bool includeStencil;
		bool dynamicDepthBias;
		uint32_t cullMode;
		uint32_t sampleCounts;
		DynamicRenderPass* renderPass;
		uint32_t subpass;
	}DynamicPipelineCreateInfo;

	typedef struct DynamicDescriptorPoolSize {
		uint32_t    type;
		uint32_t    descriptorCount;
	} DynamicDescriptorPoolSize;

	typedef struct DynamicDescriptorPoolCreateFlag
	{
		DynamicDescriptorPoolCreateFlag(API api)
		{
			switch (api)
			{
			case VULKAN:
				DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
				DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;
				DESCRIPTOR_POOL_CREATE_HOST_ONLY_BIT_EXT = VK_DESCRIPTOR_POOL_CREATE_HOST_ONLY_BIT_EXT;
				DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT_EXT = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT_EXT;
				DESCRIPTOR_POOL_CREATE_HOST_ONLY_BIT_VALVE = VK_DESCRIPTOR_POOL_CREATE_HOST_ONLY_BIT_VALVE;
				DESCRIPTOR_POOL_CREATE_FLAG_BITS_MAX_ENUM = VK_DESCRIPTOR_POOL_CREATE_FLAG_BITS_MAX_ENUM;
				break;
			case DIRECT3D12:
				break;
			}
		}

		uint32_t
			DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT{ 0 },
			DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT{ 0 },
			DESCRIPTOR_POOL_CREATE_HOST_ONLY_BIT_EXT{ 0 },
			DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT_EXT{ 0 },
			DESCRIPTOR_POOL_CREATE_HOST_ONLY_BIT_VALVE{ 0 },
			DESCRIPTOR_POOL_CREATE_FLAG_BITS_MAX_ENUM{ 0 };
	}DynamicDescriptorPoolCreateFlag;

	typedef struct DynamicDescriptorPoolCreateInfo
	{
		//VkDescriptorPoolCreateFlags    
		uint32_t                                flags;
		uint32_t                                maxSets;
		std::vector<DynamicDescriptorPoolSize>* pPoolSizes;
	}DynamicDescriptorPoolCreateInfo;

	typedef struct DynamicRenderingInfo
	{
		bool              isRenderOnSwapChain;
		uint32_t          swapChainIndex;
		bool              isClearEveryFrame;
		//if not render on swap chain, using target iamge and iamge view
		DynamicImage*      targetImage;
		DynamicImageView*  targetImageView;
		DynamicImage*      targetDepthImage;
		DynamicImageView*  targetDepthImageView;
		uint32_t           colorAspectFlag;
		uint32_t           depthAspectFlag;
		bool               includeStencil;
		uint32_t           depthImageLayout;
		uint32_t           targetImageWidth;
		uint32_t           targetImageHeight;
	}DynamicRenderingInfo;

	typedef struct DynamicAttachmentDescriptionFlags
	{
		DynamicAttachmentDescriptionFlags(API api)
		{
			switch (api)
			{
			case VULKAN:
				ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT = VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT;
				ATTACHMENT_DESCRIPTION_FLAG_BITS_MAX_ENUM = VK_ATTACHMENT_DESCRIPTION_FLAG_BITS_MAX_ENUM;

				break;
			case DIRECT3D12:
				break;
			}
		}

		uint32_t  
			ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT{ 0 },
			ATTACHMENT_DESCRIPTION_FLAG_BITS_MAX_ENUM{ 0 };
	}DynamicAttachmentDescriptionFlags;

	typedef struct DynamicAttachmentLoadOp
	{
		DynamicAttachmentLoadOp(API api)
		{
			switch (api)
			{
			case VULKAN:
				ATTACHMENT_LOAD_OP_LOAD = VK_ATTACHMENT_LOAD_OP_LOAD;
				ATTACHMENT_LOAD_OP_CLEAR = VK_ATTACHMENT_LOAD_OP_CLEAR;
				ATTACHMENT_LOAD_OP_DONT_CARE = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				//ATTACHMENT_LOAD_OP_NONE_KHR = VK_ATTACHMENT_LOAD_OP_NONE_KHR;
				ATTACHMENT_LOAD_OP_NONE_EXT = VK_ATTACHMENT_LOAD_OP_NONE_EXT;
				ATTACHMENT_LOAD_OP_MAX_ENUM = VK_ATTACHMENT_LOAD_OP_MAX_ENUM;
				break;
			case DIRECT3D12:
				break;
			}
		}

		uint32_t
			ATTACHMENT_LOAD_OP_LOAD{ 0 },
			ATTACHMENT_LOAD_OP_CLEAR{ 0 },
			ATTACHMENT_LOAD_OP_DONT_CARE{ 0 },
			ATTACHMENT_LOAD_OP_NONE_KHR{ 0 },
			ATTACHMENT_LOAD_OP_NONE_EXT{ 0 },
			ATTACHMENT_LOAD_OP_MAX_ENUM{ 0 };
	}DynamicAttachmentLoadOp;

	typedef struct DynamicAttachmentStoreOp
	{
		DynamicAttachmentStoreOp(API api)
		{
			switch (api)
			{
			case VULKAN:
				ATTACHMENT_STORE_OP_STORE = VK_ATTACHMENT_STORE_OP_STORE;
				ATTACHMENT_STORE_OP_DONT_CARE = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				ATTACHMENT_STORE_OP_NONE = VK_ATTACHMENT_STORE_OP_NONE;
				ATTACHMENT_STORE_OP_NONE_KHR = VK_ATTACHMENT_STORE_OP_NONE_KHR;
				ATTACHMENT_STORE_OP_NONE_QCOM = VK_ATTACHMENT_STORE_OP_NONE_QCOM;
				ATTACHMENT_STORE_OP_NONE_EXT = VK_ATTACHMENT_STORE_OP_NONE_EXT;
				ATTACHMENT_STORE_OP_MAX_ENUM = VK_ATTACHMENT_STORE_OP_MAX_ENUM;
				break;
			case DIRECT3D12:
				break;
			}
		}

		uint32_t
			ATTACHMENT_STORE_OP_STORE{ 0 },
			ATTACHMENT_STORE_OP_DONT_CARE{ 0 },
			ATTACHMENT_STORE_OP_NONE{ 0 },
			ATTACHMENT_STORE_OP_NONE_KHR{ 0 },
			ATTACHMENT_STORE_OP_NONE_QCOM{ 0 },
			ATTACHMENT_STORE_OP_NONE_EXT{ 0 },
			ATTACHMENT_STORE_OP_MAX_ENUM{ 0 };
	}DynamicAttachmentStoreOp;

	typedef struct DynamicAttachmentDescription
	{
		//DynamicAttachmentDescriptionFlags
		uint32_t flags;
		//DynamicFormat
		uint32_t format;
		//DynamicSampleCountFlagBits
		uint32_t samples;
		//DynamicAttachmentLoadOp             
		uint32_t loadOp;
		//DynamicAttachmentStoreOp             
		uint32_t storeOp;
		//DynamicAttachmentLoadOp              
		uint32_t stencilLoadOp;
		//DynamicAttachmentStoreOp             
		uint32_t stencilStoreOp;
		//DynamicImageLayout                   
		uint32_t initialLayout;
		//DynamicImageLayout                   
		uint32_t finalLayout;
	}DynamicAttachmentDescription;

	typedef struct DynamicAttachmentReference
	{
		uint32_t attachment;
		//VkImageLayout    
		uint32_t layout;
	}DynamicAttachmentReference;

	typedef struct DynamicSubpassDescriptionFlags
	{
		DynamicSubpassDescriptionFlags(API api)
		{
			switch (api)
			{
			case VULKAN:
				SUBPASS_DESCRIPTION_PER_VIEW_ATTRIBUTES_BIT_NVX = VK_SUBPASS_DESCRIPTION_PER_VIEW_ATTRIBUTES_BIT_NVX;
				SUBPASS_DESCRIPTION_PER_VIEW_POSITION_X_ONLY_BIT_NVX = VK_SUBPASS_DESCRIPTION_PER_VIEW_POSITION_X_ONLY_BIT_NVX;
				SUBPASS_DESCRIPTION_FRAGMENT_REGION_BIT_QCOM = VK_SUBPASS_DESCRIPTION_FRAGMENT_REGION_BIT_QCOM;
				SUBPASS_DESCRIPTION_SHADER_RESOLVE_BIT_QCOM = VK_SUBPASS_DESCRIPTION_SHADER_RESOLVE_BIT_QCOM;
				SUBPASS_DESCRIPTION_RASTERIZATION_ORDER_ATTACHMENT_COLOR_ACCESS_BIT_EXT = VK_SUBPASS_DESCRIPTION_RASTERIZATION_ORDER_ATTACHMENT_COLOR_ACCESS_BIT_EXT;
				SUBPASS_DESCRIPTION_RASTERIZATION_ORDER_ATTACHMENT_DEPTH_ACCESS_BIT_EXT = VK_SUBPASS_DESCRIPTION_RASTERIZATION_ORDER_ATTACHMENT_DEPTH_ACCESS_BIT_EXT;
				SUBPASS_DESCRIPTION_RASTERIZATION_ORDER_ATTACHMENT_STENCIL_ACCESS_BIT_EXT = VK_SUBPASS_DESCRIPTION_RASTERIZATION_ORDER_ATTACHMENT_STENCIL_ACCESS_BIT_EXT;
				SUBPASS_DESCRIPTION_ENABLE_LEGACY_DITHERING_BIT_EXT = VK_SUBPASS_DESCRIPTION_ENABLE_LEGACY_DITHERING_BIT_EXT;
				SUBPASS_DESCRIPTION_RASTERIZATION_ORDER_ATTACHMENT_COLOR_ACCESS_BIT_ARM = VK_SUBPASS_DESCRIPTION_RASTERIZATION_ORDER_ATTACHMENT_COLOR_ACCESS_BIT_ARM;
				SUBPASS_DESCRIPTION_RASTERIZATION_ORDER_ATTACHMENT_DEPTH_ACCESS_BIT_ARM = VK_SUBPASS_DESCRIPTION_RASTERIZATION_ORDER_ATTACHMENT_DEPTH_ACCESS_BIT_ARM;
				SUBPASS_DESCRIPTION_RASTERIZATION_ORDER_ATTACHMENT_STENCIL_ACCESS_BIT_ARM = VK_SUBPASS_DESCRIPTION_RASTERIZATION_ORDER_ATTACHMENT_STENCIL_ACCESS_BIT_ARM;
				SUBPASS_DESCRIPTION_FLAG_BITS_MAX_ENUM = VK_SUBPASS_DESCRIPTION_FLAG_BITS_MAX_ENUM;
				break;
			case DIRECT3D12:
				break;
			}
		}

		uint32_t
			SUBPASS_DESCRIPTION_PER_VIEW_ATTRIBUTES_BIT_NVX{ 0 },
			SUBPASS_DESCRIPTION_PER_VIEW_POSITION_X_ONLY_BIT_NVX{ 0 },
			SUBPASS_DESCRIPTION_FRAGMENT_REGION_BIT_QCOM{ 0 },
			SUBPASS_DESCRIPTION_SHADER_RESOLVE_BIT_QCOM{ 0 },
			SUBPASS_DESCRIPTION_RASTERIZATION_ORDER_ATTACHMENT_COLOR_ACCESS_BIT_EXT{ 0 },
			SUBPASS_DESCRIPTION_RASTERIZATION_ORDER_ATTACHMENT_DEPTH_ACCESS_BIT_EXT{ 0 },
			SUBPASS_DESCRIPTION_RASTERIZATION_ORDER_ATTACHMENT_STENCIL_ACCESS_BIT_EXT{ 0 },
			SUBPASS_DESCRIPTION_ENABLE_LEGACY_DITHERING_BIT_EXT{ 0 },
			SUBPASS_DESCRIPTION_RASTERIZATION_ORDER_ATTACHMENT_COLOR_ACCESS_BIT_ARM{ 0 },
			SUBPASS_DESCRIPTION_RASTERIZATION_ORDER_ATTACHMENT_DEPTH_ACCESS_BIT_ARM{ 0 },
			SUBPASS_DESCRIPTION_RASTERIZATION_ORDER_ATTACHMENT_STENCIL_ACCESS_BIT_ARM{ 0 },
			SUBPASS_DESCRIPTION_FLAG_BITS_MAX_ENUM{ 0 };
	}DynamicSubpassDescriptionFlags;

	typedef struct DynamicPipelineStageFlags
	{
		DynamicPipelineStageFlags(API api)
		{
			switch (api)
			{
			case VULKAN:
				PIPELINE_STAGE_TOP_OF_PIPE_BIT = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				PIPELINE_STAGE_DRAW_INDIRECT_BIT = VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;
				PIPELINE_STAGE_VERTEX_INPUT_BIT = VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
				PIPELINE_STAGE_VERTEX_SHADER_BIT = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
				PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT = VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT;
				PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT = VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT;
				PIPELINE_STAGE_GEOMETRY_SHADER_BIT = VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT;
				PIPELINE_STAGE_FRAGMENT_SHADER_BIT = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
				PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
				PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
				PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				PIPELINE_STAGE_COMPUTE_SHADER_BIT = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
				PIPELINE_STAGE_TRANSFER_BIT = VK_PIPELINE_STAGE_TRANSFER_BIT;
				PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
				PIPELINE_STAGE_HOST_BIT = VK_PIPELINE_STAGE_HOST_BIT;
				PIPELINE_STAGE_ALL_GRAPHICS_BIT = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
				PIPELINE_STAGE_ALL_COMMANDS_BIT = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
				PIPELINE_STAGE_NONE = VK_PIPELINE_STAGE_NONE;
				PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT = VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT;
				PIPELINE_STAGE_CONDITIONAL_RENDERING_BIT_EXT = VK_PIPELINE_STAGE_CONDITIONAL_RENDERING_BIT_EXT;
				PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR = VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;
				PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR = VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR;
				PIPELINE_STAGE_FRAGMENT_DENSITY_PROCESS_BIT_EXT = VK_PIPELINE_STAGE_FRAGMENT_DENSITY_PROCESS_BIT_EXT;
				PIPELINE_STAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR = VK_PIPELINE_STAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
				PIPELINE_STAGE_COMMAND_PREPROCESS_BIT_NV = VK_PIPELINE_STAGE_COMMAND_PREPROCESS_BIT_NV;
				PIPELINE_STAGE_TASK_SHADER_BIT_EXT = VK_PIPELINE_STAGE_TASK_SHADER_BIT_EXT;
				PIPELINE_STAGE_MESH_SHADER_BIT_EXT = VK_PIPELINE_STAGE_MESH_SHADER_BIT_EXT;
				PIPELINE_STAGE_SHADING_RATE_IMAGE_BIT_NV = VK_PIPELINE_STAGE_SHADING_RATE_IMAGE_BIT_NV;
				PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_NV = VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_NV;
				PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_NV = VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_NV;
				PIPELINE_STAGE_TASK_SHADER_BIT_NV = VK_PIPELINE_STAGE_TASK_SHADER_BIT_NV;
				PIPELINE_STAGE_MESH_SHADER_BIT_NV = VK_PIPELINE_STAGE_MESH_SHADER_BIT_NV;
				PIPELINE_STAGE_NONE_KHR = VK_PIPELINE_STAGE_NONE_KHR;
				//PIPELINE_STAGE_COMMAND_PREPROCESS_BIT_EXT = VK_PIPELINE_STAGE_COMMAND_PREPROCESS_BIT_EXT;
				PIPELINE_STAGE_FLAG_BITS_MAX_ENUM = VK_PIPELINE_STAGE_FLAG_BITS_MAX_ENUM;
				break;
			case DIRECT3D12:
				break;
			}
		}

		uint32_t
			PIPELINE_STAGE_TOP_OF_PIPE_BIT{ 0 },
			PIPELINE_STAGE_DRAW_INDIRECT_BIT{ 0 },
			PIPELINE_STAGE_VERTEX_INPUT_BIT{ 0 },
			PIPELINE_STAGE_VERTEX_SHADER_BIT{ 0 },
			PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT{ 0 },
			PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT{ 0 },
			PIPELINE_STAGE_GEOMETRY_SHADER_BIT{ 0 },
			PIPELINE_STAGE_FRAGMENT_SHADER_BIT{ 0 },
			PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT{ 0 },
			PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT{ 0 },
			PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT{ 0 },
			PIPELINE_STAGE_COMPUTE_SHADER_BIT{ 0 },
			PIPELINE_STAGE_TRANSFER_BIT{ 0 },
			PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT{ 0 },
			PIPELINE_STAGE_HOST_BIT{ 0 },
			PIPELINE_STAGE_ALL_GRAPHICS_BIT{ 0 },
			PIPELINE_STAGE_ALL_COMMANDS_BIT{ 0 },
			PIPELINE_STAGE_NONE{ 0 },
			PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT{ 0 },
			PIPELINE_STAGE_CONDITIONAL_RENDERING_BIT_EXT{ 0 },
			PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR{ 0 },
			PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR{ 0 },
			PIPELINE_STAGE_FRAGMENT_DENSITY_PROCESS_BIT_EXT{ 0 },
			PIPELINE_STAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR{ 0 },
			PIPELINE_STAGE_COMMAND_PREPROCESS_BIT_NV{ 0 },
			PIPELINE_STAGE_TASK_SHADER_BIT_EXT{ 0 },
			PIPELINE_STAGE_MESH_SHADER_BIT_EXT{ 0 },
			PIPELINE_STAGE_SHADING_RATE_IMAGE_BIT_NV{ 0 },
			PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_NV{ 0 },
			PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_NV{ 0 },
			PIPELINE_STAGE_TASK_SHADER_BIT_NV{ 0 },
			PIPELINE_STAGE_MESH_SHADER_BIT_NV{ 0 },
			PIPELINE_STAGE_NONE_KHR{ 0 },
			PIPELINE_STAGE_COMMAND_PREPROCESS_BIT_EXT{ 0 },
			PIPELINE_STAGE_FLAG_BITS_MAX_ENUM{ 0 };

	}DynamicPipelineStageFlags;

	typedef struct DynamicAccessFlagBits
	{
		DynamicAccessFlagBits(API api)
		{
			switch (api)
			{
			case VULKAN:
				ACCESS_INDIRECT_COMMAND_READ_BIT = VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
				ACCESS_INDEX_READ_BIT = VK_ACCESS_INDEX_READ_BIT;
				ACCESS_VERTEX_ATTRIBUTE_READ_BIT = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
				ACCESS_UNIFORM_READ_BIT = VK_ACCESS_UNIFORM_READ_BIT;
				ACCESS_INPUT_ATTACHMENT_READ_BIT = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
				ACCESS_SHADER_READ_BIT = VK_ACCESS_SHADER_READ_BIT;
				ACCESS_SHADER_WRITE_BIT = VK_ACCESS_SHADER_WRITE_BIT;
				ACCESS_COLOR_ATTACHMENT_READ_BIT = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
				ACCESS_COLOR_ATTACHMENT_WRITE_BIT = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
				ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				ACCESS_TRANSFER_READ_BIT = VK_ACCESS_TRANSFER_READ_BIT;
				ACCESS_TRANSFER_WRITE_BIT = VK_ACCESS_TRANSFER_WRITE_BIT;
				ACCESS_HOST_READ_BIT = VK_ACCESS_HOST_READ_BIT;
				ACCESS_HOST_WRITE_BIT = VK_ACCESS_HOST_WRITE_BIT;
				ACCESS_MEMORY_READ_BIT = VK_ACCESS_MEMORY_READ_BIT;
				ACCESS_MEMORY_WRITE_BIT = VK_ACCESS_MEMORY_WRITE_BIT;
				ACCESS_NONE = VK_ACCESS_NONE;
				ACCESS_TRANSFORM_FEEDBACK_WRITE_BIT_EXT = VK_ACCESS_TRANSFORM_FEEDBACK_WRITE_BIT_EXT;
				ACCESS_TRANSFORM_FEEDBACK_COUNTER_READ_BIT_EXT = VK_ACCESS_TRANSFORM_FEEDBACK_COUNTER_READ_BIT_EXT;
				ACCESS_TRANSFORM_FEEDBACK_COUNTER_WRITE_BIT_EXT = VK_ACCESS_TRANSFORM_FEEDBACK_COUNTER_WRITE_BIT_EXT;
				ACCESS_CONDITIONAL_RENDERING_READ_BIT_EXT = VK_ACCESS_CONDITIONAL_RENDERING_READ_BIT_EXT;
				ACCESS_COLOR_ATTACHMENT_READ_NONCOHERENT_BIT_EXT = VK_ACCESS_COLOR_ATTACHMENT_READ_NONCOHERENT_BIT_EXT;
				ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR = VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR;
				ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;
				ACCESS_FRAGMENT_DENSITY_MAP_READ_BIT_EXT = VK_ACCESS_FRAGMENT_DENSITY_MAP_READ_BIT_EXT;
				ACCESS_FRAGMENT_SHADING_RATE_ATTACHMENT_READ_BIT_KHR = VK_ACCESS_FRAGMENT_SHADING_RATE_ATTACHMENT_READ_BIT_KHR;
				ACCESS_COMMAND_PREPROCESS_READ_BIT_NV = VK_ACCESS_COMMAND_PREPROCESS_READ_BIT_NV;
				ACCESS_COMMAND_PREPROCESS_WRITE_BIT_NV = VK_ACCESS_COMMAND_PREPROCESS_WRITE_BIT_NV;
				ACCESS_SHADING_RATE_IMAGE_READ_BIT_NV = VK_ACCESS_SHADING_RATE_IMAGE_READ_BIT_NV;
				ACCESS_ACCELERATION_STRUCTURE_READ_BIT_NV = VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_NV;
				ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_NV = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_NV;
				ACCESS_NONE_KHR = VK_ACCESS_NONE_KHR;
				//ACCESS_COMMAND_PREPROCESS_READ_BIT_EXT = VK_ACCESS_COMMAND_PREPROCESS_READ_BIT_EXT;
				//ACCESS_COMMAND_PREPROCESS_WRITE_BIT_EXT = VK_ACCESS_COMMAND_PREPROCESS_WRITE_BIT_EXT;
				ACCESS_FLAG_BITS_MAX_ENUM = VK_ACCESS_FLAG_BITS_MAX_ENUM;
				break;
			case DIRECT3D12:
				break;
			}
		}

		uint32_t
			ACCESS_INDIRECT_COMMAND_READ_BIT{ 0 },
			ACCESS_INDEX_READ_BIT{ 0 },
			ACCESS_VERTEX_ATTRIBUTE_READ_BIT{ 0 },
			ACCESS_UNIFORM_READ_BIT{ 0 },
			ACCESS_INPUT_ATTACHMENT_READ_BIT{ 0 },
			ACCESS_SHADER_READ_BIT{ 0 },
			ACCESS_SHADER_WRITE_BIT{ 0 },
			ACCESS_COLOR_ATTACHMENT_READ_BIT{ 0 },
			ACCESS_COLOR_ATTACHMENT_WRITE_BIT{ 0 },
			ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT{ 0 },
			ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT{ 0 },
			ACCESS_TRANSFER_READ_BIT{ 0 },
			ACCESS_TRANSFER_WRITE_BIT{ 0 },
			ACCESS_HOST_READ_BIT{ 0 },
			ACCESS_HOST_WRITE_BIT{ 0 },
			ACCESS_MEMORY_READ_BIT{ 0 },
			ACCESS_MEMORY_WRITE_BIT{ 0 },
			ACCESS_NONE{ 0 },
			ACCESS_TRANSFORM_FEEDBACK_WRITE_BIT_EXT{ 0 },
			ACCESS_TRANSFORM_FEEDBACK_COUNTER_READ_BIT_EXT{ 0 },
			ACCESS_TRANSFORM_FEEDBACK_COUNTER_WRITE_BIT_EXT{ 0 },
			ACCESS_CONDITIONAL_RENDERING_READ_BIT_EXT{ 0 },
			ACCESS_COLOR_ATTACHMENT_READ_NONCOHERENT_BIT_EXT{ 0 },
			ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR{ 0 },
			ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR{ 0 },
			ACCESS_FRAGMENT_DENSITY_MAP_READ_BIT_EXT{ 0 },
			ACCESS_FRAGMENT_SHADING_RATE_ATTACHMENT_READ_BIT_KHR{ 0 },
			ACCESS_COMMAND_PREPROCESS_READ_BIT_NV{ 0 },
			ACCESS_COMMAND_PREPROCESS_WRITE_BIT_NV{ 0 },
			ACCESS_SHADING_RATE_IMAGE_READ_BIT_NV{ 0 },
			ACCESS_ACCELERATION_STRUCTURE_READ_BIT_NV{ 0 },
			ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_NV{ 0 },
			ACCESS_NONE_KHR{ 0 },
			ACCESS_COMMAND_PREPROCESS_READ_BIT_EXT{ 0 },
			ACCESS_COMMAND_PREPROCESS_WRITE_BIT_EXT{ 0 },
			ACCESS_FLAG_BITS_MAX_ENUM{ 0 };
	}DynamicAccessFlagBits;

	typedef struct DynamicDependencyFlagBits
	{
		DynamicDependencyFlagBits(API api)
		{
			switch (api)
			{
			case VULKAN:
				DEPENDENCY_BY_REGION_BIT = VK_DEPENDENCY_BY_REGION_BIT;
				DEPENDENCY_DEVICE_GROUP_BIT = VK_DEPENDENCY_DEVICE_GROUP_BIT;
				DEPENDENCY_VIEW_LOCAL_BIT = VK_DEPENDENCY_VIEW_LOCAL_BIT;
				DEPENDENCY_FEEDBACK_LOOP_BIT_EXT = VK_DEPENDENCY_FEEDBACK_LOOP_BIT_EXT;
				DEPENDENCY_VIEW_LOCAL_BIT_KHR = VK_DEPENDENCY_VIEW_LOCAL_BIT_KHR;
				DEPENDENCY_DEVICE_GROUP_BIT_KHR = VK_DEPENDENCY_DEVICE_GROUP_BIT_KHR;
				DEPENDENCY_FLAG_BITS_MAX_ENUM = VK_DEPENDENCY_FLAG_BITS_MAX_ENUM;
				break;
			case DIRECT3D12:
				break;
			}
		}

		uint32_t
			DEPENDENCY_BY_REGION_BIT{ 0 },
			DEPENDENCY_DEVICE_GROUP_BIT{ 0 },
			DEPENDENCY_VIEW_LOCAL_BIT{ 0 },
			DEPENDENCY_FEEDBACK_LOOP_BIT_EXT{ 0 },
			DEPENDENCY_VIEW_LOCAL_BIT_KHR{ 0 },
			DEPENDENCY_DEVICE_GROUP_BIT_KHR{ 0 },
			DEPENDENCY_FLAG_BITS_MAX_ENUM{ 0 };
	}DynamicDependencyFlagBits;

	enum DynamicCommandBufferLevel
	{
		PRIMARY,
		SECONDARY
	};

	typedef struct DynamicSubpassDescription
	{
		//DynamicSubpassDescriptionFlags       
		uint32_t                          flags;
		//DynamicPipelineBindPoint             
		uint32_t                          pipelineBindPoint;
		uint32_t                          inputAttachmentCount;
		const DynamicAttachmentReference* pInputAttachments;
		uint32_t                          colorAttachmentCount;
		const DynamicAttachmentReference* pColorAttachments;
		const DynamicAttachmentReference* pResolveAttachments;
		const DynamicAttachmentReference* pDepthStencilAttachment;
		uint32_t                          preserveAttachmentCount;
		const uint32_t*                   pPreserveAttachments;
	}DynamicSubpassDescription;

	typedef struct DynamicSubpassDependency
	{
		uint32_t                srcSubpass;
		uint32_t                dstSubpass;
		//DynamicPipelineStageFlags    
		uint32_t srcStageMask;
		//DynamicPipelineStageFlags   
		uint32_t dstStageMask;
		//DynamicAccessFlags           
		uint32_t srcAccessMask;
		//DynamicAccessFlags           
		uint32_t dstAccessMask;
		//DynamicDependencyFlags       
		uint32_t dependencyFlags;
	}DynamicSubpassDependency;

	typedef struct DynamicRenderPassCreateFlagBits
	{
		DynamicRenderPassCreateFlagBits(API api)
		{
			switch (api)
			{
			case VULKAN:
				RENDER_PASS_CREATE_TRANSFORM_BIT_QCOM = VK_RENDER_PASS_CREATE_TRANSFORM_BIT_QCOM;
				RENDER_PASS_CREATE_FLAG_BITS_MAX_ENUM = VK_RENDER_PASS_CREATE_FLAG_BITS_MAX_ENUM;
				break;
			case DIRECT3D12:
				break;
			}
		}

		uint32_t
			RENDER_PASS_CREATE_TRANSFORM_BIT_QCOM{ 0 },
			RENDER_PASS_CREATE_FLAG_BITS_MAX_ENUM{ 0 };
	}DynamicRenderPassCreateFlagBits;

	typedef struct DynamicSubpassContents 
	{
		DynamicSubpassContents(API api)
		{
			switch (api)
			{
			case VULKAN:
				SUBPASS_CONTENTS_INLINE = VK_SUBPASS_CONTENTS_INLINE;
				SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS = VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS;
				//SUBPASS_CONTENTS_INLINE_AND_SECONDARY_COMMAND_BUFFERS_KHR = VK_SUBPASS_CONTENTS_INLINE_AND_SECONDARY_COMMAND_BUFFERS_KHR;
				SUBPASS_CONTENTS_INLINE_AND_SECONDARY_COMMAND_BUFFERS_EXT = VK_SUBPASS_CONTENTS_INLINE_AND_SECONDARY_COMMAND_BUFFERS_EXT;
				SUBPASS_CONTENTS_MAX_ENUM = VK_SUBPASS_CONTENTS_MAX_ENUM;
				break;
			case DIRECT3D12:
				break;
			}
		}
		uint32_t
			SUBPASS_CONTENTS_INLINE{ 0 },
			SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS{ 0 },
			SUBPASS_CONTENTS_INLINE_AND_SECONDARY_COMMAND_BUFFERS_KHR{ 0 },
			SUBPASS_CONTENTS_INLINE_AND_SECONDARY_COMMAND_BUFFERS_EXT{ 0 },
			SUBPASS_CONTENTS_MAX_ENUM{ 0 };
	}DynamicSubpassContents;
	
	typedef struct DynamicRenderPassCreateInfo
	{
		//DynamicRenderPassCreateFlags           
		uint32_t                               flags;
		uint32_t                               attachmentCount;
		const DynamicAttachmentDescription*    pAttachments;
		uint32_t                               subpassCount;
		const DynamicSubpassDescription*       pSubpasses;
		uint32_t                               dependencyCount;
		std::vector<DynamicSubpassDependency>* pDependencies;
	}DynamicRenderPassCreateInfo;

	typedef struct DynamicRenderPassBeginInfo
	{
		DynamicRenderPass      renderPass;
		DynamicFramebuffer     framebuffer;
		DynamicRect2D          renderArea;
	}DynamicRenderPassBeginInfo;

	typedef struct DynamicFramebufferCreateFlagBits
	{
		DynamicFramebufferCreateFlagBits(API api)
		{
			switch (api)
			{
			case VULKAN:
				FRAMEBUFFER_CREATE_IMAGELESS_BIT = VK_FRAMEBUFFER_CREATE_IMAGELESS_BIT;
				FRAMEBUFFER_CREATE_IMAGELESS_BIT_KHR = VK_FRAMEBUFFER_CREATE_IMAGELESS_BIT_KHR;
				FRAMEBUFFER_CREATE_FLAG_BITS_MAX_ENUM = VK_FRAMEBUFFER_CREATE_FLAG_BITS_MAX_ENUM;
				break;
			case DIRECT3D12:
				break;
			}
		}

		uint32_t
			FRAMEBUFFER_CREATE_IMAGELESS_BIT{ 0 },
			FRAMEBUFFER_CREATE_IMAGELESS_BIT_KHR{ 0 },
			FRAMEBUFFER_CREATE_FLAG_BITS_MAX_ENUM{ 0 };
	}DynamicFramebufferCreateFlagBits;

	typedef struct DynamicFramebufferCreateInfo
	{
		//DynamicFramebufferCreateFlags    
		uint32_t                flags;
		DynamicRenderPass       renderPass;
		uint32_t                attachmentCount;
		DynamicImageView*       pAttachments;
		uint32_t                width;
		uint32_t                height;
		uint32_t                layers;
	}DynamicFramebufferCreateInfo;

	typedef struct DynamicViewport 
	{
		float    x;
		float    y;
		float    width;
		float    height;
		float    minDepth;
		float    maxDepth;
	}DynamicViewport;

	typedef struct DynamicOffset3D 
	{
		int32_t    x;
		int32_t    y;
		int32_t    z;
	} DynamicOffset3D;


	typedef struct DynamicImageSubresourceLayers
	{
		//DynamicImageAspectFlags
		uint32_t    aspectMask;
		uint32_t    mipLevel;
		uint32_t    baseArrayLayer;
		uint32_t    layerCount;
	} DynamicImageSubresourceLayers;

	typedef struct DynamicImageCopy 
	{
		DynamicImageSubresourceLayers    srcSubresource;
		DynamicOffset3D                  srcOffset;
		DynamicImageSubresourceLayers    dstSubresource;
		DynamicOffset3D                  dstOffset;
		DynamicExtent3D                  extent;
	} Dynamic;

	typedef struct RenderingInfo
	{
		VkImage     colorImage;
		VkImageView colorImageView;
		uint32_t    colorWidth;
		uint32_t    colorHeight;
		VkImage     depthImage;
		VkImageView depthImageView;
		uint32_t    depthWidth;
		uint32_t    depthHeight;
		bool        isClearEveryFrame;
	}RenderingInfo;

	//ray tracing
	typedef VkDeviceAddress DynamicDeviceAddress;

	typedef union DynamicDeviceOrHostAddressConst 
	{
		DynamicDeviceAddress    deviceAddress;
		const void*             hostAddress;
	} DynamicDeviceOrHostAddressConst;

	typedef struct DynamicAccelerationStructureGeometryTrianglesData
	{
		uint32_t                         vertexFormat;
		DynamicDeviceOrHostAddressConst  vertexData;
		uint64_t                         vertexStride;
		uint32_t                         maxVertex;
		uint32_t                         indexType;
		DynamicDeviceOrHostAddressConst  indexData;
		DynamicDeviceOrHostAddressConst  transformData;
	}DynamicAccelerationStructureGeometryTrianglesData;

	typedef struct DynamicGeometryType
	{
		DynamicGeometryType(API api)
		{
			switch (api)
			{
			case VULKAN:
				GEOMETRY_TYPE_TRIANGLES_KHR = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
				GEOMETRY_TYPE_AABBS_KHR = VK_GEOMETRY_TYPE_AABBS_KHR;
				GEOMETRY_TYPE_INSTANCES_KHR = VK_GEOMETRY_TYPE_INSTANCES_KHR;
				GEOMETRY_TYPE_TRIANGLES_NV = VK_GEOMETRY_TYPE_TRIANGLES_NV;
				GEOMETRY_TYPE_AABBS_NV = VK_GEOMETRY_TYPE_AABBS_NV;
				GEOMETRY_TYPE_MAX_ENUM_KHR = VK_GEOMETRY_TYPE_MAX_ENUM_KHR;
				break;
			case DIRECT3D12:
				break;
			}
		}

		uint32_t
			GEOMETRY_TYPE_TRIANGLES_KHR{ 0 },
			GEOMETRY_TYPE_AABBS_KHR{ 0 },
			GEOMETRY_TYPE_INSTANCES_KHR{ 0 },
			GEOMETRY_TYPE_TRIANGLES_NV{ 0 },
			GEOMETRY_TYPE_AABBS_NV{ 0 },
			GEOMETRY_TYPE_MAX_ENUM_KHR{ 0 };

	} DynamicGeometryType;

	typedef struct DynamicAccelerationStructureGeometryAabbsData
	{
		DynamicDeviceOrHostAddressConst   data;
		uint64_t                          stride;
	} DynamicAccelerationStructureGeometryAabbsData;

	typedef struct DynamicAccelerationStructureGeometryInstancesData
	{
		uint32_t                        arrayOfPointers;
		DynamicDeviceOrHostAddressConst data;
	} DynamicAccelerationStructureGeometryInstancesData;

	typedef union DynamicAccelerationStructureGeometryData 
	{
		DynamicAccelerationStructureGeometryTrianglesData  triangles;
		DynamicAccelerationStructureGeometryAabbsData      aabbs;
		DynamicAccelerationStructureGeometryInstancesData  instances;
	} DynamicAccelerationStructureGeometryData;

	typedef struct DynamicGeometryFlagBits
	{
		DynamicGeometryFlagBits(API api)
		{
			switch (api)
			{
			case VULKAN:
				GEOMETRY_OPAQUE_BIT_KHR = VK_GEOMETRY_OPAQUE_BIT_KHR;
				GEOMETRY_NO_DUPLICATE_ANY_HIT_INVOCATION_BIT_KHR = VK_GEOMETRY_NO_DUPLICATE_ANY_HIT_INVOCATION_BIT_KHR;
				GEOMETRY_OPAQUE_BIT_NV = VK_GEOMETRY_OPAQUE_BIT_KHR;
				GEOMETRY_NO_DUPLICATE_ANY_HIT_INVOCATION_BIT_NV = VK_GEOMETRY_NO_DUPLICATE_ANY_HIT_INVOCATION_BIT_NV;
				GEOMETRY_FLAG_BITS_MAX_ENUM_KHR = VK_GEOMETRY_FLAG_BITS_MAX_ENUM_KHR;
				break;
			case DIRECT3D12:
				break;
			}
		}

		uint32_t
			GEOMETRY_OPAQUE_BIT_KHR{ 0 },
			GEOMETRY_NO_DUPLICATE_ANY_HIT_INVOCATION_BIT_KHR{ 0 },
			GEOMETRY_OPAQUE_BIT_NV{ 0 },
			GEOMETRY_NO_DUPLICATE_ANY_HIT_INVOCATION_BIT_NV{ 0 },
			GEOMETRY_FLAG_BITS_MAX_ENUM_KHR{ 0 };

	}DynamicGeometryFlagBits;
 
	typedef struct DynamicAccelerationStructureGeometryKHR 
	{
		//DynamicGeometryType
		uint32_t                         geometryType;
		DynamicAccelerationStructureGeometryData    geometry;
		//DynamicGeometryFlagBits
		uint32_t                        flags;
	} DynamicAccelerationStructureGeometryKHR;
}




