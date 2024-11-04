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

		//auto s = std::make_shared<Sphere>(30, 30, 5.0f);
		//s->_material = std::make_shared<BasicMaterial>(texture2);
		//add(s);

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

		//for (auto object : _objs)
		//{
		//	object->updateUniformBuffer(_camera, _light);
		//}

		for (auto object : _group)
		{
			object->updateUniformBuffer(_camera, _light);
		}
	}
}