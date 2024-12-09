#pragma once

#include<memory>
#include<unordered_map>

#include"BlinnPhongMaterial.h"
#include"../Texture.h"

namespace FOCUS
{
	class MaterialManager
	{
	public:
		MaterialManager() = default;

		std::shared_ptr<BlinnPhongMaterial> createMaterial(std::string name, std::shared_ptr<Texture> texture);
		std::shared_ptr<BlinnPhongMaterial> getMaterialByName(std::string name);

	private:

		std::unordered_map<std::string, std::shared_ptr<BlinnPhongMaterial>> _globalMaterials;
	};
}

namespace std
{
	template<> struct hash<FOCUS::BlinnPhongMaterial>
	{
		size_t operator()(FOCUS::BlinnPhongMaterial const& material) const
		{
			return std::hash<std::string>()(material._name);
		}
	};
}