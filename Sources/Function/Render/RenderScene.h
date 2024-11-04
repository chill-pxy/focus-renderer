#pragma once

#include<memory>
#include<vector>

#include"../../Core/Math.h"
#include"Mesh.h"
#include"RenderResource.h"
#include"PointLight.h"

namespace FOCUS
{
	class RenderScene
	{
	public:
		std::vector<RenderResource*> _group;
		std::vector<std::shared_ptr<Mesh>> _objs;
		std::shared_ptr<PointLight> _light;
		std::shared_ptr<RenderCamera> _camera;

	public:
		RenderScene() = default;

		void initialize();
		void prepareRenderResources();
		void add(RenderResource* resource);
		void tick(float frameTimer);
	};
}