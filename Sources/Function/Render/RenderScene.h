#pragma once

#include<memory>
#include<vector>

#include"../../Core/Math.h"
#include"Mesh.h"
#include"Overlay.h"
#include"RenderResource.h"

namespace FOCUS
{
	class RenderScene
	{
	public:
		std::vector<RenderResource*> _group;
		std::vector<std::shared_ptr<Mesh>> _objs;

	public:
		RenderScene() = default;

		void initialize();
		void prepareRenderResources();
		void add(RenderResource* resource);
		void tick(uint32_t currentImage, std::shared_ptr<RenderCamera> camera);
	};
}