#pragma once

#include<memory>
#include<vector>

#include"../../Core/Math.h"
#include"Geometry/Mesh.h"
#include"RenderResource.h"
#include"PointLight.h"

namespace FOCUS
{
	class RenderScene
	{
	public:
		std::vector<std::shared_ptr<RenderResource>> _group;
		std::shared_ptr<PointLight> _light;
		std::shared_ptr<RenderCamera> _camera;

	public:
		RenderScene() = default;

		void initialize();
		void prepareRenderResources();
		void add(std::shared_ptr<RenderResource> resource);
		void tick(float frameTimer);
	};
}