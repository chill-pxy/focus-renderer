#include"RenderScene.h"
#include"Materials/BasicMaterial.h"
#include"Materials/BlinnPhongMaterial.h"
#include"Materials/PhysicalMaterial.h"
#include"Materials/EnvironmentMap.h"
#include"Geometry/Model.h"
#include"Geometry/Box.h"
#include"SkyCube.h"
#include"../IO/Loader/ObjLoader.h"

namespace focus
{
	template<class T>
	void removeSameName(std::shared_ptr<T>& resource, std::vector<std::shared_ptr<T>>& group)
	{
		uint32_t count = 0;
		for (uint32_t i = 0; i < group.size(); ++i)
		{
			if (resource->_name == group[i]->_name)
			{
				count++;
				resource->_name = resource->_name + std::to_string(count);
				i = 0;
			}
		}
	}

	void RenderScene::initialize(std::shared_ptr<drhi::DynamicRHI> rhi)
	{
		_pointLight = std::make_shared<PointLight>();
		_dirLight = std::make_shared<DirectionalLight>();
		_camera = std::make_shared<RenderCamera>();

		_materialManager = std::make_shared<MaterialManager>();

		prepareRenderResources(rhi);
	}

	void RenderScene::prepareRenderResources(std::shared_ptr<drhi::DynamicRHI> rhi)
	{
		// prepare light
		_pointLight->_position = Vector3(0.0f, 55.0f, 0.0f);
		_pointLight->_intensity = 1000.0f;
		_pointLight->_name = "PointLight";

		_dirLight->_intensity = 1.0f;
		add(_pointLight);

		// prepare camera
        _camera->_position = Vector3(9, 41, -202);
        _camera->_rotation = Vector3(-9, -361, 0);

		// prepare obj
		//auto obj = loadModel("../../../Asset/Models/sponza/sponza.obj");
		//addModel(obj);

        auto obj2 = loadModel(RESOURCE_PATH"Asset/Models/defaultPlaneW.obj", "plane");
        for (auto& m : obj2->_meshes)
        {
            m->_material->_metallic = 0.98;
            m->_material->_roughness = 0.98;
            m->_castShadow = false;
            m->_scale = Vector3(0.1, 0.1, 0.1);
            m->_position = Vector3(0, -5.0, 0);
        }
        addModel(obj2);

        //for (int i = 0; i < 10; ++i)
        //{
        //    //for (int j = 0; j < 10; ++j)
        //    {
        //        auto sphere = std::make_shared<Sphere>();
        //        auto texture = loadTexture(RESOURCE_PATH"Asset/Images/white.png");//loadTexture("../../../Asset/Images/white.png");
        //        sphere->_material = std::make_shared<PhysicalMaterial>(texture);
        //        sphere->_material->_metallic = (clamp((float)i / (float)10, 0.005f, 1.0f));
        //        sphere->_material->_roughness = 1.0;//1.0f - clamp((float)i / (float)10, 0.005f, 1.0f);
        //        sphere->_scale = Vector3(10.0, 10.0, 10.0);
        //        sphere->_position = Vector3(i * 25, 10.0, -50);
        //        sphere->_name = "obj" + std::to_string(i);
        //        add(sphere);
        //    }
        //}

        //auto cube = loadModel("../../../Asset/Models/box.obj");
        //add(cube); 

        //auto teapot = loadModel(RESOURCE_PATH"Asset/Models/teapot.obj");//loadModel("../../../Asset/Models/teapot.obj");
        //teapot->setMetallic(0.987);// ((clamp((float)i / (float)10, 0.005f, 1.0f)));
        //teapot->setRoughness(0.012);// (1.0f - clamp((float)i / (float)10, 0.005f, 1.0f));
        //teapot->setPosition(Vector3(0.0, 20.0, -105.0));// (Vector3(30 * i, 10, 0));
        //teapot->setScale(Vector3(10.0, 10.0, 10.0));
        //addModel(teapot);

        //auto sphere = loadModel(RESOURCE_PATH"Asset/Models/dragon.obj");//loadModel("../../../Asset/Models/dragon.obj");
        //sphere->setMetallic(0.987);// ((clamp((float)i / (float)10, 0.005f, 1.0f)));
        //sphere->setRoughness(0.012);// (1.0f - clamp((float)i / (float)10, 0.005f, 1.0f));
        //sphere->setPosition(Vector3(-80.0, 30.0, 0.0));// (Vector3(30 * i, 10, 0));
        //sphere->setScale(Vector3(10.0, 10.0, 10.0));
        //addModel(sphere);

        //auto box = std::make_shared<Box>();
        //auto texture2 = loadTexture("../../../Asset/Images/white.png");
        //box->_material = std::make_shared<PhysicalMaterial>(texture2);
        //box->_material->_metallic = 0.98;
        //box->_material->_roughness = 0.11;
        //box->_scale = Vector3(50.0, 50.0, 50.0);
        //box->_position = Vector3(0, 25.0, 40);
        //box->_name = "box";
        //add(box);
	}

	void RenderScene::add(std::shared_ptr<RenderResource> resource)
	{
		if (_t_name_table.find(resource->_name) != _t_name_table.end())
		{
			_t_name_table[resource->_name]++;
			resource->_name = resource->_name + std::to_string(_t_name_table[resource->_name]);
		}
		else
		{
			_t_name_table[resource->_name] = 0;
		}

		_submitGroup.push_back(resource);

		_group.push_back(resource);
	}

	void RenderScene::addModel(std::shared_ptr<Model> model)
	{
		for (auto mesh : model->_meshes)
		{
			//auto m = static_cast<std::shared_ptr<RenderResource>>(mesh);
			//removeSameName(m, _submitGroup);

			if (_t_name_table.find(mesh->_name) != _t_name_table.end())
			{
				_t_name_table[mesh->_name]++;
				mesh->_name = mesh->_name + std::to_string(_t_name_table[mesh->_name]);
			}
			else
			{
				_t_name_table[mesh->_name] = 0;
			}

			_submitGroup.push_back(mesh);
		}

		//removeSameName(model, _modelGroup);

		if (_t_name_table.find(model->_name) != _t_name_table.end())
		{
			_t_name_table[model->_name]++;
			model->_name = model->_name + std::to_string(_t_name_table[model->_name]);
		}
		else
		{
			_t_name_table[model->_name] = 0;
		}

		_modelGroup.push_back(model);
	}

	void RenderScene::tick(float frameTimer)
	{
		// camera tick
		_camera->handleMovement(frameTimer);

		_uud.view = _camera->getViewMatrix();
		_uud.proj = perspective(radians(60.0f), _canvasWidth / (float)_canvasHeight, 1.0f, 10000.0f);
		_uud.proj[1][1] *= -1;

		_uud.viewPosition = _camera->_position * Vector3(-1.0f, -1.0f, -1.0f);

		_uud.pointLightPosition = _pointLight->_position * -1.0f;
		_uud.pointLightColor = _pointLight->_color;
		_uud.pointLightIntensity = _pointLight->_intensity;

		_uud.dirLightDirection =  -_dirLight->_target;
        _uud.dirLightPosition = _dirLight->_position;
		_uud.dirLightColor = _dirLight->_color;
		_uud.dirLightStrength = _dirLight->_intensity;

		for (auto object : _submitGroup)
		{
			object->updateUniformBuffer(_uud);
		}
	}

	void RenderScene::clean(std::shared_ptr<drhi::DynamicRHI> rhi)
	{
		// clean render resources
		for (auto r : _submitGroup)
		{
			rhi->clearBuffer(&r->_indexBuffer, &r->_indexDeviceMemory);
			rhi->clearBuffer(&r->_vertexBuffer, &r->_vertexDeviceMemory);

			r->_material->clean(rhi);
            r->_shadow->clean(rhi);
            r->_deffered->clean(rhi);
		}
	}

    std::shared_ptr<Model> RenderScene::loadModel(std::string modelPath, std::string name)
    {
		auto type = modelPath.substr(modelPath.find("."));

		if (type == ".obj")
		{
			ObjLoader loader{};
			return loader.loadModel(modelPath, _materialManager, name);
		}
		else
		{
			return nullptr;
		}
    }
}