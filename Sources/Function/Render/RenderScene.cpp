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

		// prepare scene
        auto obj = loadModel(RESOURCE_PATH"Asset/Models/defaultPlaneW.obj", "plane");
        for (auto& m : obj->_meshes)
        {
            m->_material->_metallic = 0.98;
            m->_material->_roughness = 0.98;
            m->_castShadow = false;
            m->_scale = Vector3(0.1, 0.1, 0.1);
            m->_position = Vector3(0, -5.0, 0);
        }
        addModel(obj);
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
		_camera->tick(frameTimer);

		_uud.view = _camera->getViewMatrix();
		_uud.proj = _camera->getProjMatrix();

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