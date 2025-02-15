#pragma once

#include<vector>

#include"../RenderResource.h"
#include"MeshVertex.h"

namespace FOCUS
{
	class Box : public RenderResource
	{
	public:
		std::vector<Vertex> _vertices{};
		std::vector<uint32_t> _indices{};

		double _width{};
		double _length{};
		double _height{};

	public:
		Box(double width, double length, double height) : _width(width), _length(length), _height(height){};
	
		virtual void build(std::shared_ptr<DRHI::DynamicRHI> rhi, DRHI::DynamicCommandPool* commandPool, DRHI::DynamicImage shadowImage, DRHI::DynamicImageView shadowImageView, DRHI::DynamicSampler shadowSampler)
		{
			// create vertices
			_vertices = {
				(-_width / 2, -_length / 2, _height / 2),
				(_width / 2, -_length / 2, _height / 2),
				(_width / 2, _length / 2, _height / 2),
				(-_width / 2, _length / 2, _height / 2),
				(-_width / 2, -_length / 2, -_height / 2),
				(_width / 2, -_length / 2, _height / 2),
				(_width / 2, _length / 2, -_height / 2),
				(-_width / 2, _length / 2, -_height / 2),
			};

			_indices = {
				0, 1, 2,
				2, 3, 0,
				1, 5, 6,
				6, 2, 1,
				5, 4, 7,
				7, 6, 5,
				4, 0, 3,
				3, 7, 4,
				3, 2, 6,
				6, 7, 3,
				4, 5, 1,
				1, 0, 4,
			};

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