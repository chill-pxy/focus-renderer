#include"RenderScene.h"
#include"Materials/BasicMaterial.h"
#include"Materials/BlinnPhongMaterial.h"
#include"Geometry/Sphere.h"

namespace FOCUS
{
	void RenderScene::initialize()
	{
		_light = std::make_shared<PointLight>();
		_camera = std::make_shared<RenderCamera>();

		prepareRenderResources();
	}

	void RenderScene::prepareRenderResources()
	{
		// prepare obj
		auto obj = loadModel("../../../Asset/Models/viking_room.obj");
		auto texture = loadTexture("../../../Asset/Models/viking_room.png");
		obj->_material = std::make_shared<BlinnPhongMaterial>(texture);
		add(obj);

		auto plane = loadModel("../../../Asset/Models/plane.obj");
		auto texture2 = loadTexture("../../../Asset/Models/plane.png");
		plane->_material = std::make_shared<BasicMaterial>(texture2);
		add(plane);

		auto s = std::make_shared<Sphere>(60, 60, 1.0f);
		auto texture3 = loadTexture("../../../Asset/Models/plane.png");
		s->_material = std::make_shared<BasicMaterial>(texture3);
		add(s);

		// prepare light
		_light->_position = Vector3(0.0f, 10.0f, 0.0f);
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
		uud.model = Matrix4(1.0f);
		uud.view = _camera->getViewMatrix();
		uud.proj = perspective(radians(45.0f), 1280 / (float)720, 0.1f, 100.0f);
		uud.proj[1][1] *= -1;
		uud.lightPosition = _light->_position;
		uud.viewPosition = _camera->_position;

		for (auto object : _group)
		{
			object->updateUniformBuffer(uud);
		}
	}
}