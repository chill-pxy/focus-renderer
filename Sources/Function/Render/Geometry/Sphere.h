#pragma once

#include<vector>

#include"../../../Core/Math.h"
#include"../RenderResource.h"

namespace FOCUS
{
	typedef struct SphereVertex
	{
		Vector3 pos;
		Vector3 color;
	}SphereVertex;

	class Sphere : public RenderResource
	{
	public:
		std::vector<SphereVertex> _vertices{};
		std::vector<uint32_t> _indices{};

		uint32_t _latBands{};
		uint32_t _lonBands{};
		float _radius{ 0.0f };

	public:
		Sphere() = default;

		Sphere(uint32_t latBands, uint32_t lonBands, float radius) : _latBands(latBands), _lonBands(lonBands), _radius(radius) {}

		virtual void build(std::shared_ptr<DRHI::DynamicRHI> rhi)
		{
			// create vertices
			uint32_t count = 0;
			for (int i = 0; i < _latBands; ++i)
			{
				for (int j = 0; j < _lonBands; ++j)
				{
					float latitude = (float)i / _latBands * PI;
					float longtitude = (float)j / _lonBands * 2 * PI;

					float x = _radius * sin(latitude) * cos(longtitude);
					float y = _radius * sin(latitude) * sin(latitude);
					float z = _radius * cos(latitude);

					SphereVertex vertex{};
					vertex.pos = Vector3(x, y, z);
					vertex.color = Vector3(1.0f, 1.0f, 1.0f);
					
					_vertices.push_back(vertex);
					_indices.push_back(count);
					count++;
				}
			}

			auto api = rhi->getCurrentAPI();
			auto bufferUsage = DRHI::DynamicBufferUsageFlags(api);
			auto memoryFlags = DRHI::DynamicMemoryPropertyFlagBits(api);

			//create vertex buffer
			auto vertexBufferSize = sizeof(_vertices[0]) * _vertices.size();
			rhi->createDynamicBuffer(&_vertexBuffer, &_vertexDeviceMemory, vertexBufferSize, _vertices.data(), bufferUsage.BUFFER_USAGE_VERTEX_BUFFER_BIT, memoryFlags.MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

			//create index buffer
			auto indexBufferSize = sizeof(_indices[0]) * _indices.size();
			rhi->createDynamicBuffer(&_indexBuffer, &_indexDeviceMemory, indexBufferSize, _indices.data(), bufferUsage.BUFFER_USAGE_INDEX_BUFFER_BIT, memoryFlags.MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		
			_material->build(rhi);
		}

		virtual void draw(uint32_t index, std::shared_ptr<DRHI::DynamicRHI> rhi)
		{
			auto api = rhi->getCurrentAPI();
			auto indexType = DRHI::DynamicIndexType(api);

			_material->draw(index, rhi);

			rhi->bindVertexBuffers(&_vertexBuffer, index);
			rhi->bindIndexBuffer(&_indexBuffer, index, indexType.INDEX_TYPE_UINT32);

			//draw model
			rhi->drawIndexed(index, static_cast<uint32_t>(_indices.size()), 1, 0, 0, 0);
		}

		virtual void updateUniformBuffer(std::shared_ptr<RenderCamera> camera, std::shared_ptr<PointLight> light)
		{
			_material->updateUniformBuffer(camera, light);
		}
	};
}