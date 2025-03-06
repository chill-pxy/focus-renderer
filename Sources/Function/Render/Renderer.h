#pragma once

#include<memory>
#include<vector>

#include<drhi.h>

#include"../../Core/Math.h"
#include"RenderResource.h"
#include"SkySphere.h"
#include"SkyCube.h"
#include"DefferedPipeline.h"

namespace FOCUS
{
	class Renderer
	{
	public:
		std::shared_ptr<DRHI::DynamicRHI> _rhiContext;
		std::vector<std::shared_ptr<RenderResource>> _submitRenderlist;
		
		std::vector<DRHI::DynamicCommandBuffer> _commandBuffers;
		DRHI::DynamicCommandPool _commandPool;

		std::vector<DRHI::DynamicCommandBuffer> _shadowCommandBuffers;
		DRHI::DynamicCommandPool _shadowCommandPool;

		// scene image
		std::vector<DRHI::DynamicImage>* _viewportImages{nullptr};
		std::vector<DRHI::DynamicImageView>* _viewportImageViews{nullptr};
		
		// scene depth image
		DRHI::DynamicImage* _viewportDepthImage{nullptr};
		DRHI::DynamicImageView* _viewportDepthImageView{nullptr};
		
		// scene normal image
		DRHI::DynamicImage* _viewportNormalImage{ nullptr };
		DRHI::DynamicImageView* _viewportNormalImageView{ nullptr };

		// environment map
		std::shared_ptr<SkyCube> _environmentMap{};
		DRHI::DynamicCommandPool _environmentMapCommandPool{};

		std::shared_ptr<DefferedPipeline> _deffered{};

		bool _prepared = false;

	private:	
		// members of shadow
		DRHI::DynamicImage        _shadowImage{};
		DRHI::DynamicDeviceMemory _shadowImageMemory{};
		DRHI::DynamicImageView    _shadowImageView{};
		DRHI::DynamicSampler      _shadowSampler{};

		uint32_t _shadowDepthImageWidth{ 2048 };
		uint32_t _shadowDepthImageHeight{ 2048 };

		// members of brdf lut
		DRHI::DynamicImage        _brdflutImage{};
		DRHI::DynamicDeviceMemory _brdflutImageMemory{};
		DRHI::DynamicImageView    _brdflutImageView{};
		DRHI::DynamicSampler      _brdflutSampler{};

		// members of irradiance map
		DRHI::DynamicImage        _irradianceImage{};
		DRHI::DynamicDeviceMemory _irradianceImageMemory{};
		DRHI::DynamicImageView    _irradianceImageView{};
		DRHI::DynamicSampler      _irradianceSampler{};

		// members of prefilteredImage
		uint32_t _filteredImageWidth{};
		uint32_t _filteredImageHeight{};
		DRHI::DynamicImage        _filteredImage{};
		DRHI::DynamicDeviceMemory _filteredImageMemory{};
		DRHI::DynamicImageView    _filteredImageView{};
		DRHI::DynamicSampler      _filteredImageSampler{};

	public:
		Renderer() = delete;

		Renderer(DRHI::API api, DRHI::PlatformInfo platformCI);
		
		void initialize();
		void buildCommandBuffer();
		void buildAndSubmit(std::vector<std::shared_ptr<RenderResource>>* renderlist, std::vector<DRHI::DynamicCommandBuffer>* commandBuffers, DRHI::DynamicCommandPool* commandPool);
		void submitRenderTargetImage(std::vector<DRHI::DynamicImage>* viewportImages, std::vector<DRHI::DynamicImageView>* viewportImageViews, DRHI::DynamicImage* depthImage, DRHI::DynamicImageView* depthImageView);
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