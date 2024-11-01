#include"RenderScene.h"
#include"Materials/BasicMaterial.h"
#include"Materials/BlinnPhongMaterial.h"

namespace FOCUS
{
	void RenderScene::initialize()
	{
		prepareRenderResources();
	}

	void RenderScene::prepareRenderResources()
	{
		// prepare obj
		auto obj = std::shared_ptr<Mesh>(loadModel("../../../Asset/Models/viking_room.obj"));

		std::shared_ptr<Texture> texture = std::shared_ptr<Texture>(loadTexture("../../../Asset/Models/viking_room.png"));
		obj->_material = new BasicMaterial(texture);

		_objs.push_back(obj);

		auto plane = std::shared_ptr<Mesh>(loadModel("../../../Asset/Models/plane.obj"));

		std::shared_ptr<Texture> texture2 = std::shared_ptr<Texture>(loadTexture("../../../Asset/Models/plane.png"));
		plane->_material = new BlinnPhongMaterial(texture2);

		_objs.push_back(plane);

		for (auto value : _objs)
		{
			add(value.get());
		}
	}

	void RenderScene::add(RenderResource* resource)
	{
		_group.push_back(resource);
	}

	void RenderScene::tick(uint32_t currentImage, std::shared_ptr<RenderCamera> camera)
	{
		for (auto object : _objs)
		{
			object->updateUniformBuffer(currentImage, camera);
		}
	}
}