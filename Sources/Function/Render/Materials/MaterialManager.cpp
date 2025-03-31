#include "MaterialManager.h"

namespace focus
{
	std::shared_ptr<PhysicalMaterial> MaterialManager::createMaterial(std::string name, std::shared_ptr<Texture> texture)
	{
		if (_globalMaterials.count(name) == 0)
		{
			std::shared_ptr<PhysicalMaterial> material = std::make_shared<PhysicalMaterial>(texture);
			_globalMaterials.insert({ name, material });
			return material;
		}
		else
		{
			return _globalMaterials[name];
		}
	}

	std::shared_ptr<PhysicalMaterial> MaterialManager::getMaterialByName(std::string name)
	{
		return _globalMaterials[name];
	}
}