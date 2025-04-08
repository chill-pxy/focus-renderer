#pragma once

#include<unordered_map>

#include"Mesh.h"
#include"../Materials/BlinnPhongMaterial.h"
#include"../Materials/PhysicalMaterial.h"
#include"../Texture.h"

namespace focus
{
	class Model
	{
    public:
		std::string _name{"undefined"};
        std::vector<std::shared_ptr<Mesh>> _meshes;
        std::vector<std::shared_ptr<PhysicalMaterial>> _materials;

		void setRoughness(float value)
		{
			for (auto& m : _meshes)
			{
				m->_material->_roughness = value;
			}
		}

		void setMetallic(float value)
		{
			for (auto& m : _meshes)
			{
				m->_material->_metallic = value;
			}
		}

		void setPosition(Vector3 position)
		{
			for (auto& m : _meshes)
			{
				m->_position += position;
			}
		}

		void setScale(Vector3 scale)
		{
			for (auto& m : _meshes)
			{
				m->_scale = scale;
			}
		}
	};
}