#include"RenderScene.h"
#include"Materials/BasicMaterial.h"

namespace FOCUS
{
	void RenderScene::initialize()
	{
		prepareRenderResources();
	}

	void RenderScene::prepareRenderResources()
	{
		// prepare obj
		_obj = std::shared_ptr<Mesh>(loadModel("../../../Asset/Models/viking_room.obj"));

		std::shared_ptr<Texture> texture = std::shared_ptr<Texture>(loadTexture("../../../Asset/Models/viking_room.png"));
		_obj->_material = new BasicMaterial(texture);

		add(_obj.get());
	}

	void RenderScene::add(RenderResource* resource)
	{
		_group.push_back(resource);
	}

	void RenderScene::tick(uint32_t currentImage, Matrix4 view)
	{
		_obj->updateUniformBuffer(currentImage, view);
	}
}