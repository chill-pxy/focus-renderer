module;
#include<memory>
#include<iostream>

#include"GlobalContext.h"
#include"Core/Path.h"
#include"Engine.h"
export module Engine;

namespace FOCUS
{
	export EngineSingleton;
}

extern "C" __declspec(dllexport) void run()
{
	std::thread t([] {
		auto app = FOCUS::EngineSingleton::getInstance();
		app->initialize();
		app->run();
	});

	t.detach();
}

extern "C" __declspec(dllexport) int getSceneObjCount()
{
	return FOCUS::RenderSystemSingleton::getInstance()->_scene->_group.size();
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

extern "C" __declspec(dllexport) void plantObj(const char* path)
{
	auto scene = FOCUS::RenderSystemSingleton::getInstance()->_scene;
	auto obj = scene->loadModel(path);
	obj->setMetallic(0.987);
	obj->setRoughness(0.012);
	obj->setPosition(FOCUS::Vector3(0.0, 0.0, 0.0));
	obj->setScale(FOCUS::Vector3(10.0, 10.0, 10.0));

	//for thread safe, sumbit after engine stop committing commands
	FOCUS::EngineSingleton::getInstance()->stop();
	while (1)
	{
		if (FOCUS::EngineSingleton::getInstance()->getJobState())
		{
			scene->addModel(obj);
			FOCUS::RenderSystemSingleton::getInstance()->_renderer->buildAndSubmit(&scene->_group, &scene->_sceneCommandBuffers, &scene->_sceneCommandPool);
			break;
		}
	}
			
	FOCUS::EngineSingleton::getInstance()->start();
}