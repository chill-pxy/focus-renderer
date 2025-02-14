#pragma once

#include<vector>

#include"../RenderResource.h"
#include"MeshVertex.h"

namespace FOCUS
{
	class Sphere : public RenderResource
	{
	public:
		std::vector<Vertex> _vertices{};
		std::vector<uint32_t> _indices{};

		uint32_t _latBands{};
		uint32_t _lonBands{};

		float _radius{ 0.0f };

	public:
		Sphere() : _latBands(60), _lonBands(60), _radius(1.0f){};

		Sphere(uint32_t latBands, uint32_t lonBands, float radius) : _latBands(latBands), _lonBands(lonBands), _radius(radius) {}

		virtual void build(std::shared_ptr<DRHI::DynamicRHI> rhi, DRHI::DynamicCommandPool* commandPool, DRHI::DynamicImage shadowImage, DRHI::DynamicImageView shadowImageView, DRHI::DynamicSampler shadowSampler)
		{
			// create vertices
			for (int i = 0; i <= _latBands; ++i)
			{
				for (int j = 0; j <= _lonBands; ++j)
				{
					float latitude = (i / (float)_latBands) * PI;
					float longitude = (j / (float)_lonBands) * 2.0f * PI;

					float x = _radius * sin(latitude) * cos(longitude);
					float y = _radius * sin(latitude) * sin(longitude);
					float z = _radius * cos(latitude);

					Vertex vertex{};
					vertex.pos = Vector3(x, y, z);
					vertex.normal = Vector3(x, y, z);
					vertex.texCoord = Vector2(j / (float)_lonBands, i / (float)_latBands);
					vertex.color = _color;
					_vertices.push_back(vertex);

					uint32_t first = (i * (_lonBands + 1)) + j;
					uint32_t second = first + _lonBands + 1;

					_indices.push_back(first);
					_indices.push_back(second);
					_indices.push_back(first + 1);

					_indices.push_back(second);
					_indices.push_back(second + 1);
					_indices.push_back(first + 1);
				}
			}

			auto api = rhi->getCurrentAPI();
			auto bufferUsage = DRHI::DynamicBufferUsageFlags(api);
			auto memoryFlags = DRHI::DynamicMemoryPropertyFlagBits(api);

			//create vertex buffer
			auto vertexBufferSize = sizeof(_vertices[0]) * _vertices.size();
			rhi->createDynamicBuffer(&_vertexBuffer, &_vertexDeviceMemory, commandPool, vertexBufferSize, _vertices.data(), bufferUsage.BUFFER_USAGE_VERTEX_BUFFER_BIT, memoryFlags.MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

			//create index buffer
			auto indexBufferSize = sizeof(_indices[0]) * _indices.size();
			rhi->createDynamicBuffer(&_indexBuffer, &_indexDeviceMemory, commandPool, indexBufferSize, _indices.data(), bufferUsage.BUFFER_USAGE_INDEX_BUFFER_BIT, memoryFlags.MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		
			//initiailize shadow map
			_shadow = std::make_shared<ShadowMap>();
			_shadow->initialize(rhi, commandPool);

			_material->build(rhi, commandPool, shadowImage, shadowImageView, shadowSampler);
		}

		virtual void draw(std::shared_ptr<DRHI::DynamicRHI> rhi, DRHI::DynamicCommandBuffer* commandBuffer, bool isShdaowPass)
		{
			auto api = rhi->getCurrentAPI();
			auto indexType = DRHI::DynamicIndexType(api);

			if (isShdaowPass)
			{
				_shadow->draw(rhi, commandBuffer);
			}
			else
			{
				_material->draw(rhi, commandBuffer);
			}
			
			rhi->bindVertexBuffers(&_vertexBuffer, commandBuffer);
			rhi->bindIndexBuffer(&_indexBuffer, commandBuffer, indexType.INDEX_TYPE_UINT32);

			//draw model
			rhi->drawIndexed(commandBuffer ,static_cast<uint32_t>(_indices.size()), 1, 0, 0, 0);
		}

		virtual void draw(std::shared_ptr<DRHI::DynamicRHI> rhi, DRHI::DynamicCommandBuffer* commandBuffer, DRHI::DynamicPipeline pipeline, DRHI::DynamicPipelineLayout pipelineLayout, DRHI::DynamicDescriptorSet set)
		{
			auto api = rhi->getCurrentAPI();
			auto indexType = DRHI::DynamicIndexType(api);
			auto bindPoint = DRHI::DynamicPipelineBindPoint(api);

			rhi->bindPipeline(pipeline, commandBuffer, bindPoint.PIPELINE_BIND_POINT_GRAPHICS);
			rhi->bindDescriptorSets(&set, pipelineLayout, commandBuffer, bindPoint.PIPELINE_BIND_POINT_GRAPHICS);

			rhi->bindVertexBuffers(&_vertexBuffer, commandBuffer);
			rhi->bindIndexBuffer(&_indexBuffer, commandBuffer, indexType.INDEX_TYPE_UINT32);

			//draw model
			rhi->drawIndexed(commandBuffer, static_cast<uint32_t>(_indices.size()), 1, 0, 0, 0);
		}

		virtual void updateUniformBuffer(UniformUpdateData uud)
		{
			uud.vertexColor = _color;

			// handle scale
			auto scaleMatrix = scale(identity<Matrix4>(), Vector3(_scale.x, _scale.y, _scale.z));

			// handle translate
			auto transMatrix = translate(identity<Matrix4>(), _position);

			uud.model = transMatrix * scaleMatrix * _rotation;

			_shadow->updateUniform(uud);
			_material->updateUniformBuffer(uud);
		}
	};
}