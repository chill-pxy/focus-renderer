#include"RenderScene.h"
#include"Materials/BasicMaterial.h"
#include"Materials/BlinnPhongMaterial.h"
#include"Geometry/Sphere.h"

namespace FOCUS
{
	void RenderScene::initialize(std::shared_ptr<DRHI::DynamicRHI> rhi)
	{
		rhi->createCommandPool(&_sceneCommandPool);
		rhi->createCommandBuffers(&_sceneCommandBuffers, &_sceneCommandPool);

		_light = std::make_shared<PointLight>();
		_camera = std::make_shared<RenderCamera>();

		prepareRenderResources();
	}

	void RenderScene::prepareRenderResources()
	{
		// prepare light
		_light->_position = Vector3(0.0f, 0.6f, 0.0f);
		add(_light);

		// prepare camera
		//_camera->_position = Vector3(-1.54f, 2.41f, 3.21f);

		// prepare obj
		auto obj = loadModel("../../../Asset/Models/viking_room.obj");
		auto texture = loadTexture("../../../Asset/Models/viking_room.png");
		obj->_material = std::make_shared<BlinnPhongMaterial>(texture);
		add(obj);

		auto plane = loadModel("../../../Asset/Models/plane.obj");
		auto texture2 = loadTexture("../../../Asset/Models/plane.png");
		plane->_material = std::make_shared<BlinnPhongMaterial>(texture2);
		add(plane);


	}

	void RenderScene::add(std::shared_ptr<RenderResource> resource)
	{
		_group.push_back(resource);
	}

	void RenderScene::tick(float frameTimer)
	{
		// camera tick
		_camera->handleMovement(frameTimer);

		UniformUpdateData uud{};
		auto identity = Matrix4(1.0f);
		uud.view = _camera->getViewMatrix();
		uud.proj = perspective(radians(45.0f), _canvasWidth / (float)_canvasHeight, 0.1f, 100.0f);
		uud.proj[1][1] *= -1;
		uud.lightPosition = _light->_position;
		uud.viewPosition = _camera->_position;
		uud.lightColor = _light->_color;
		uud.lightIntensity = _light->_intensity;
		
		for (auto object : _group)
		{
			uud.vertexColor = object->_color;
			uud.model = translate(identity, object->_position);
			object->updateUniformBuffer(uud);
		}
	}
}