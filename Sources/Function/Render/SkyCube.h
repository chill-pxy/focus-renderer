#pragma once

#include"../Materials/EnvironmentMap.h"
#include"../Geometry/Box.h"

namespace FOCUS
{
	class SkyCube : Box
	{
	public:
		std::shared_ptr<EnvironmentMap> _material;

	public:
		void initialize(std::shared_ptr<DRHI::DynamicRHI> rhi, DRHI::DynamicCommandPool* commandPool)
		{
			_material->build(rhi, commandPool, nullptr, nullptr, nullptr);
		}

		virtual void draw(std::shared_ptr<DRHI::DynamicRHI> rhi, DRHI::DynamicCommandBuffer* commandBuffer)
		{
			auto api = rhi->getCurrentAPI();
			auto indexType = DRHI::DynamicIndexType(api);

			_material->draw(rhi, commandBuffer);

			rhi->bindVertexBuffers(&_vertexBuffer, commandBuffer);
			rhi->bindIndexBuffer(&_indexBuffer, commandBuffer, indexType.INDEX_TYPE_UINT32);

			//draw model
			rhi->drawIndexed(commandBuffer, static_cast<uint32_t>(_indices.size()), 1, 0, 0, 0);
		}

		virtual void updateUniformBuffer(UniformUpdateData uud)
		{
			_material->updateUniformBuffer(uud);
		}
	};
}