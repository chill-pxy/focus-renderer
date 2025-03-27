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

extern "C" __declspec(dllexport) void plantObj()
{
	auto scene = FOCUS::RenderSystemSingleton::getInstance()->_scene;
	auto teapot = scene->loadModel(RESOURCE_PATH"Asset/Models/teapot.obj");//loadModel("../../../Asset/Models/teapot.obj");
	teapot->setMetallic(0.987);// ((clamp((float)i / (float)10, 0.005f, 1.0f)));
	teapot->setRoughness(0.012);// (1.0f - clamp((float)i / (float)10, 0.005f, 1.0f));
	teapot->setPosition(FOCUS::Vector3(0.0, 20.0, -75.0));// (Vector3(30 * i, 10, 0));
	teapot->setScale(FOCUS::Vector3(10.0, 10.0, 10.0));

	FOCUS::EngineSingleton::getInstance()->stop();
	while (1)
	{
		if (FOCUS::EngineSingleton::getInstance()->getJobState())
		{
			scene->addModel(teapot);
			FOCUS::RenderSystemSingleton::getInstance()->_renderer->buildAndSubmit(&scene->_group, &scene->_sceneCommandBuffers, &scene->_sceneCommandPool);
			break;
		}
	}
			
	FOCUS::EngineSingleton::getInstance()->start();
}