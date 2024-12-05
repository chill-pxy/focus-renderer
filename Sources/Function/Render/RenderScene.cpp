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

		_pointLight = std::make_shared<PointLight>();
		_dirLight = std::make_shared<DirectionalLight>();
		_camera = std::make_shared<RenderCamera>();

		prepareRenderResources();
	}

	void RenderScene::prepareRenderResources()
	{
		// prepare light
		_pointLight->_position = Vector3(0.0f, 7.0f, 0.0f);
		_pointLight->_intensity = 100.0f;
		_pointLight->_name = "PointLight";

		_dirLight->_direction = Vector3(0.0f, 0.0f, 0.0f);
		_dirLight->_intensity = 1.0f;
		add(_pointLight);

		// prepare camera
		_camera->_position = Vector3(-6.5f, 5.0f, 7.5f);
		_camera->_rotation = Vector3(-26.0f, -147.0f, 0.0f);

		// prepare obj
		auto obj = loadModel("../../../Asset/Models/sponza/sponza.obj");
		auto texture = loadTexture("../../../Asset/Models/viking_room.png");
		obj->_material = std::make_shared<BlinnPhongMaterial>(texture);
		obj->_name = "Viking Room";
		add(obj);

		auto dragon = loadModel("../../../Asset/Models/dragon.obj");
		auto texture2 = loadTexture("../../../Asset/Models/box.png");
		dragon->_material = std::make_shared<BlinnPhongMaterial>(texture2);
		dragon->_name = "Dragon";
		add(dragon);

		//auto box = loadModel("../../../Asset/Models/box.obj");
		//auto texture3 = loadTexture("../../../Asset/Models/box.png");
		//box->_material = std::make_shared<BlinnPhongMaterial>(texture3);
		//box->_name = "Box";
		//add(box);
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

		uud.viewPosition = _camera->_position;

		uud.pointLightPosition = _pointLight->_position;
		uud.pointLightColor = _pointLight->_color;
		uud.pointLightIntensity = _pointLight->_intensity;

		uud.dirLightDirection = _dirLight->_direction;
		uud.dirLightColor = _dirLight->_color;
		uud.dirLightStrength = _dirLight->_intensity;
		
		for (auto object : _group)
		{
			uud.vertexColor = object->_color;
			uud.model = translate(identity, object->_position);
			object->updateUniformBuffer(uud);
		}
	}

	void RenderScene::clean(std::shared_ptr<DRHI::DynamicRHI> rhi)
	{
		// clean render resources
		for (auto r : _group)
		{
			rhi->clearBuffer(&r->_indexBuffer, &r->_indexDeviceMemory);
			rhi->clearBuffer(&r->_vertexBuffer, &r->_vertexDeviceMemory);

			r->_material->clean(rhi);
		}

		rhi->freeCommandBuffers(&_sceneCommandBuffers, &_sceneCommandPool);
		rhi->destroyCommandPool(&_sceneCommandPool);
	}
}