#pragma once

#include<memory>
#include<vector>

#include<drhi.h>

#include"../../Core/Math.h"
#include"RenderResource.h"
#include"SkyCube.h"

namespace focus
{
	class Renderer
	{
	public:
		std::shared_ptr<drhi::DynamicRHI> _rhiContext;
		std::vector<std::shared_ptr<RenderResource>> _submitRenderlist;
		
		std::vector<drhi::DynamicCommandBuffer> _commandBuffers;
		drhi::DynamicCommandPool _commandPool;

		// shadow command buffer
		std::vector<drhi::DynamicCommandBuffer> _shadowCommandBuffers;
		drhi::DynamicCommandPool _shadowCommandPool;

		// deffered command buffer
		drhi::DynamicCommandBuffer _defferedCommandBuffer{};
		drhi::DynamicCommandPool _defferedCommandPool{};

		// scene image
		std::vector<drhi::DynamicImage>* _viewportImages{nullptr};
		std::vector<drhi::DynamicImageView>* _viewportImageViews{nullptr};
		
		// scene depth image
		drhi::DynamicImage* _viewportDepthImage{nullptr};
		drhi::DynamicImageView* _viewportDepthImageView{nullptr};

		// environment map
		std::shared_ptr<SkyCube> _environmentMap{};
		drhi::DynamicCommandPool _environmentMapCommandPool{};

		bool _prepared = false;

	private:	
		// members of shadow
		drhi::DynamicImage        _shadowImage{};
		drhi::DynamicDeviceMemory _shadowImageMemory{};
		drhi::DynamicImageView    _shadowImageView{};
		drhi::DynamicSampler      _shadowSampler{};

		uint32_t _shadowDepthImageWidth{ 2048 };
		uint32_t _shadowDepthImageHeight{ 2048 };

		// members of deffered
		drhi::DynamicImage _normal{};
		drhi::DynamicImageView _normalView{};
		drhi::DynamicSampler _normalSampler{};
		drhi::DynamicDeviceMemory _normalMemory{};

		drhi::DynamicImage _depth{};
		drhi::DynamicImageView _depthView{};
		drhi::DynamicSampler _depthSampler{};
		drhi::DynamicDeviceMemory _depthMemory{};

		// members of brdf lut
		drhi::DynamicImage        _brdflutImage{};
		drhi::DynamicDeviceMemory _brdflutImageMemory{};
		drhi::DynamicImageView    _brdflutImageView{};
		drhi::DynamicSampler      _brdflutSampler{};

		// members of irradiance map
		drhi::DynamicImage        _irradianceImage{};
		drhi::DynamicDeviceMemory _irradianceImageMemory{};
		drhi::DynamicImageView    _irradianceImageView{};
		drhi::DynamicSampler      _irradianceSampler{};

		// members of prefilteredImage
		uint32_t _filteredImageWidth{};
		uint32_t _filteredImageHeight{};
		drhi::DynamicImage        _filteredImage{};
		drhi::DynamicDeviceMemory _filteredImageMemory{};
		drhi::DynamicImageView    _filteredImageView{};
		drhi::DynamicSampler      _filteredImageSampler{};

	public:
		Renderer() = delete;

		Renderer(drhi::API api, drhi::PlatformInfo platformCI);
		
		void initialize();
		void buildCommandBuffer();
		void buildAndSubmit(std::vector<std::shared_ptr<RenderResource>>* renderlist, std::vector<drhi::DynamicCommandBuffer>* commandBuffers, drhi::DynamicCommandPool* commandPool);
		void submitRenderTargetImage(std::vector<drhi::DynamicImage>* viewportImages, std::vector<drhi::DynamicImageView>* viewportImageViews, drhi::DynamicImage* depthImage, drhi::DynamicImageView* depthImageView);
		void clean();
		void recreate();

	private:
		void shadowPass();
		void scenePass();
		void defferedPass();

		void precomputeBRDFLUT();
		void precomputeIrradianceCube();
		void prefilterEnvironmentCube();
	};
}