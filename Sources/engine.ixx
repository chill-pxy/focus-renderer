module;
#include<memory>
#include<iostream>

#include"GlobalContext.h"
#include"Core/Path.h"
#include"Engine.h"
export module Engine;

namespace focus
{
	export EngineSingleton;
}

extern "C" __declspec(dllexport) void run()
{
	std::thread t([] {
		auto app = focus::EngineSingleton::getInstance();
		app->initialize();
		app->run();
	});

	t.detach();
}

extern "C" __declspec(dllexport) int getSceneObjCount()
{
	return focus::RenderSystemSingleton::getInstance()->_scene->_group.size();
}

extern "C" __declspec(dllexport) const char* getModelPathList()
{
	const char* list =
		RESOURCE_PATH"Asset/Models/teapot.obj"
		RESOURCE_PATH"Asset/Models/cube.obj"
		RESOURCE_PATH"Asset/Models/dragon.obj"
		RESOURCE_PATH"Asset/Models/sphere.obj"
		RESOURCE_PATH"Asset/Models/viking_room.obj"
	;

	return list;
}

extern "C" __declspec(dllexport) void plantObj(const char* path, const char* name)
{
	auto scene = focus::RenderSystemSingleton::getInstance()->_scene;
	auto obj = scene->loadModel(path);
	obj->setMetallic(0.987);
	obj->setRoughness(0.012);
	obj->setPosition(focus::Vector3(0.0, 0.0, 0.0));
	obj->setScale(focus::Vector3(10.0, 10.0, 10.0));

	//for thread safe, sumbit after engine stop committing commands
	focus::EngineSingleton::getInstance()->stop();
	while (1)
	{
		if (focus::EngineSingleton::getInstance()->getJobState())
		{
			scene->addModel(obj);
			focus::RenderSystemSingleton::getInstance()->_renderer->buildAndSubmit(&scene->_submitGroup, &scene->_sceneCommandBuffers, &scene->_sceneCommandPool);
			break;
		}
	}
			
	focus::EngineSingleton::getInstance()->start();
}

extern "C" __declspec(dllexport) void changeObjPosition(const char* name, float x, float y, float z)
{
	auto scene = focus::RenderSystemSingleton::getInstance()->_scene;
	for (auto obj : scene->_group)
	{
		if (strcmp(name, obj->_name.c_str()) == 0)
		{
			obj->_position = focus::Vector3(x, y, z);
		}
	}
}

extern "C" __declspec(dllexport) void changeObjScale(const char* name, float x, float y, float z)
{
	auto scene = focus::RenderSystemSingleton::getInstance()->_scene;
	for (auto obj : scene->_group)
	{
		if (strcmp(name, obj->_name.c_str()) == 0)
		{
			obj->_scale = focus::Vector3(x, y, z);
		}
	}
}