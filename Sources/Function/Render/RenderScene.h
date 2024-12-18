#pragma once

#include<memory>
#include<vector>

#include"Geometry/Model.h"
#include"RenderResource.h"
#include"PointLight.h"
#include"DirectionalLight.h"
#include"RenderCamera.h"
#include"Materials/MaterialManager.h"
#include"ShadowMap.h"

namespace FOCUS
{
	class RenderScene
	{
	private:
		MaterialManager _materialManager;

	public:
		
		std::vector<DRHI::DynamicCommandBuffer> _sceneCommandBuffers;
		DRHI::DynamicCommandPool _sceneCommandPool;

		std::vector<std::shared_ptr<RenderResource>> _group;
		std::shared_ptr<PointLight> _pointLight;
		std::shared_ptr<DirectionalLight> _dirLight;
		std::shared_ptr<RenderCamera> _camera;

		UniformUpdateData _uud{};

		uint32_t _canvasWidth{1280};
		uint32_t _canvasHeight{720};

	public:
		RenderScene() = default;

		void initialize(std::shared_ptr<DRHI::DynamicRHI> rhi);
		void prepareRenderResources();
		void add(std::shared_ptr<RenderResource> resource);
		void add(std::shared_ptr<Model> model);
		void tick(float frameTimer, std::shared_ptr<ShadowMap> shadowMap);
		void clean(std::shared_ptr<DRHI::DynamicRHI> rhi);

		std::shared_ptr<Model> loadModel(std::string modelPath);
	};
}