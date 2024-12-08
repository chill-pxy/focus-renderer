#pragma once

#include<memory>
#include<unordered_map>

#include"Material.h"

namespace FOCUS
{
	class MaterialManager
	{
	public:
		MaterialManager() = default;

		std::shared_ptr<Material> createMaterial(std::shared_ptr<Texture> texture);

	private:

		std::unordered_map<Material> _globalMaterials;
	};
}