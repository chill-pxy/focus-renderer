#pragma once

#include<memory>
#include<unordered_map>

#include"BlinnPhongMaterial.h"
#include"PhysicalMaterial.h"
#include"../Texture.h"

namespace focus
{
	class MaterialManager
	{
	public:
		MaterialManager() = default;

		std::shared_ptr<PhysicalMaterial> createMaterial(std::string name, std::shared_ptr<Texture> texture);
		std::shared_ptr<PhysicalMaterial> getMaterialByName(std::string name);

	private:

		std::unordered_map<std::string, std::shared_ptr<PhysicalMaterial>> _globalMaterials;
	};
}

namespace std
{
	template<> struct hash<focus::PhysicalMaterial>
	{
		size_t operator()(focus::PhysicalMaterial const& material) const
		{
			return std::hash<std::string>()(material._name);
		}
	};
}