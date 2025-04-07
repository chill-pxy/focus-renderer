#pragma once

#include<memory>
#include<vector>
#include<mutex>

#include"Geometry/Model.h"
#include"RenderResource.h"
#include"PointLight.h"
#include"DirectionalLight.h"
#include"RenderCamera.h"
#include"Materials/MaterialManager.h"

namespace focus
{
	class RenderScene
	{
	private:
		std::shared_ptr<MaterialManager> _materialManager;

	public:
		std::mutex _mutex;

		std::vector<drhi::DynamicCommandBuffer> _sceneCommandBuffers;
		drhi::DynamicCommandPool _sceneCommandPool;

		std::vector<std::shared_ptr<RenderResource>> _group;
		std::shared_ptr<PointLight> _pointLight;
		std::shared_ptr<DirectionalLight> _dirLight;
		std::shared_ptr<RenderCamera> _camera;

		UniformUpdateData _uud{};

		uint32_t _canvasWidth{1280};
		uint32_t _canvasHeight{720};

	public:
		RenderScene() = default;

		void initialize(std::shared_ptr<drhi::DynamicRHI> rhi);
		void prepareRenderResources(std::shared_ptr<drhi::DynamicRHI> rhi);
		void add(std::shared_ptr<RenderResource> resource);
		void addModel(std::shared_ptr<Model> model);
		void tick(float frameTimer);
		void clean(std::shared_ptr<drhi::DynamicRHI> rhi);

		std::shared_ptr<Model> loadModel(std::string modelPath);
	};
}