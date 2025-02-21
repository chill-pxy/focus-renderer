#pragma once

#include<unordered_map>

#include"Mesh.h"
#include"../Materials/BlinnPhongMaterial.h"
#include"../Materials/PhysicalMaterial.h"
#include"../Texture.h"

namespace FOCUS
{
	class Model
	{
    public:
        std::vector<std::shared_ptr<Mesh>> _meshes;
        std::vector<std::shared_ptr<PhysicalMaterial>> _materials;
	};
}