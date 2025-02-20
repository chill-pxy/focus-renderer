#include"RenderScene.h"
#include"Materials/BasicMaterial.h"
#include"Materials/BlinnPhongMaterial.h"
#include"Materials/EnvironmentMap.h"
#include"Geometry/Model.h"
#include"Geometry/Box.h"

namespace FOCUS
{
	void RenderScene::initialize(std::shared_ptr<DRHI::DynamicRHI> rhi)
	{
		rhi->createCommandPool(&_sceneCommandPool);
		rhi->createCommandBuffers(&_sceneCommandBuffers, &_sceneCommandPool);

		_pointLight = std::make_shared<PointLight>();
		_dirLight = std::make_shared<DirectionalLight>();
		_camera = std::make_shared<RenderCamera>();

		prepareRenderResources(rhi);
	}

	void RenderScene::prepareRenderResources(std::shared_ptr<DRHI::DynamicRHI> rhi)
	{
		// prepare light
		_pointLight->_position = Vector3(0.0f, 55.0f, 0.0f);
		_pointLight->_intensity = 1000.0f;
		_pointLight->_name = "PointLight";

		_dirLight->_intensity = 1.0f;
		add(_pointLight);

		// prepare camera
        _camera->_position = Vector3(0, 50, 0);
        _camera->_rotation = Vector3(-14, -92, 0);

		// prepare obj
		auto obj = loadModel("../../../Asset/Models/sponza/sponza.obj");
		add(obj);

        auto obj2 = loadModel("../../../Asset/Models/defaultPlaneW.obj");
        for (auto& m : obj2->_meshes)
        {
            m->_castShadow = false;
        }
        add(obj2);

        //auto obj2 = loadModel("../../../Asset/Models/plane.obj");
        //add(obj2);
	}

	void RenderScene::add(std::shared_ptr<RenderResource> resource)
	{
		_group.push_back(resource);
	}

	void RenderScene::add(std::shared_ptr<Model> model)
	{
		for (auto mesh : model->_meshes)
		{
			_group.push_back(mesh);
		}
	}

	void RenderScene::tick(float frameTimer)
	{
		// camera tick
		_camera->handleMovement(frameTimer);

		_uud.view = _camera->getViewMatrix();
		_uud.proj = perspective(radians(45.0f), _canvasWidth / (float)_canvasHeight, 1.0f, 10000.0f);
		_uud.proj[1][1] *= -1;

		_uud.viewPosition = _camera->_position;

		_uud.pointLightPosition = _pointLight->_position;
		_uud.pointLightColor = _pointLight->_color;
		_uud.pointLightIntensity = _pointLight->_intensity;

		_uud.dirLightDirection = -(_dirLight->_position - _dirLight->_target);
        _uud.dirLightPosition = _dirLight->_position;
		_uud.dirLightColor = _dirLight->_color;
		_uud.dirLightStrength = _dirLight->_intensity;

		for (auto object : _group)
		{
			object->updateUniformBuffer(_uud);
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
            r->_shadow->clean(rhi);
		}

		rhi->freeCommandBuffers(&_sceneCommandBuffers, &_sceneCommandPool);
		rhi->destroyCommandPool(&_sceneCommandPool);
	}

    std::shared_ptr<Model> RenderScene::loadModel(std::string modelPath)
    {
        std::shared_ptr<Model> model = std::make_shared<Model>();

        // find mlt path
        std::string basePath;
        size_t pos = modelPath.rfind('/');
        if (pos != std::string::npos)
        {
            basePath = modelPath.substr(0, pos);
        }

        // load obj data
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, modelPath.c_str(), basePath.c_str()))
        {
            throw std::runtime_error(warn + err);
        }

        // handle material
        for (tinyobj::material_t& mat : materials)
        {
            std::shared_ptr<BlinnPhongMaterial> material;

            if (mat.diffuse_texname.compare(""))
            {
                std::shared_ptr<Texture> texture = loadTexture((basePath + '/' + mat.diffuse_texname).c_str());                
                material = _materialManager.createMaterial(mat.name, texture);

                material->_name = mat.name;
                material->_ambient = mat.ambient[0];
                material->_diffuse = mat.diffuse[0];
                material->_specular = mat.specular[0];
                material->_roughness = shininessToRoughness(mat.shininess);
                material->_shinness = mat.shininess;
                material->_ior = mat.ior;

                material->_ambientTex = mat.ambient_texname;
                material->_diffuseTex = mat.diffuse_texname;
                material->_specularTex = mat.specular_texname;

                model->_materials.push_back(material);
            }
        }

        // handle every shape
        std::vector<std::vector<tinyobj::index_t>> allSubFace;
        allSubFace.resize(materials.size());

        for (uint32_t i = 0; i < shapes.size(); ++i)
        {
            uint32_t indexCount = shapes[i].mesh.indices.size();
            uint32_t faceCount = shapes[i].mesh.num_face_vertices.size();

            uint32_t indexOffset = 0;

            for (uint32_t j = 0; j < faceCount; ++j)
            {
                int materialIndex = shapes[i].mesh.material_ids[j];

                std::vector<tinyobj::index_t>& subIdx = allSubFace[materialIndex];
                subIdx.push_back(shapes[i].mesh.indices[indexOffset++]);
                subIdx.push_back(shapes[i].mesh.indices[indexOffset++]);
                subIdx.push_back(shapes[i].mesh.indices[indexOffset++]);
            }
        }

        // handle mesh
        std::unordered_map<Vertex, uint32_t> uniqueVertices{};
        uint32_t count = 0;

        for (std::vector<tinyobj::index_t>& subIdx : allSubFace)
        {
            std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
            
            for (tinyobj::index_t& index : subIdx)
            {
                Vertex vertex{};

                if (attrib.vertices.size() > 0)
                {
                    vertex.pos =
                    {
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2]
                    };
                }

                if (attrib.texcoords.size() > 0)
                {
                    vertex.texCoord =
                    {
                        attrib.texcoords[2 * index.texcoord_index + 0],
                        1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
                    };
                }

                if (attrib.normals.size() > 0)
                {
                    vertex.normal =
                    {
                        attrib.normals[3 * index.normal_index + 0],
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2]
                    };
                }

                vertex.color = { 1.0f, 1.0f, 1.0f };

                if (uniqueVertices.count(vertex) == 0)
                {
                    uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                    vertices.push_back(vertex);
                }

                indices.push_back(uniqueVertices[vertex]);
            }

            mesh->_indices = indices;
            mesh->_vertices = vertices;
            mesh->_material = model->_materials[count];
            mesh->_name = model->_materials[count]->_name;
            mesh->_scale = Vector3(0.1, 0.1, 0.1);

            model->_meshes.push_back(mesh);

            if (count != model->_materials.size() - 1)
            {
                count++;
            }
            
        }

        return model;
    }
}