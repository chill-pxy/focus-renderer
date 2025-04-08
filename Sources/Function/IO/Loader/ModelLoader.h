#pragma once

#include"../../Render/RenderResource.h"
#include"../../Render/Materials/MaterialManager.h"

namespace focus
{
	class ModelLoader
	{
	public:
		ModelLoader() = default;
		~ModelLoader() = default;

		virtual std::shared_ptr<Model> loadModel(const std::string& path, std::shared_ptr<MaterialManager> materialManager, std::string setname = "undefined")
		{
			//loader base
			return nullptr;
		}
	};
}