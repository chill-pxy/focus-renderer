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
		Box() : _width(1), _length(1), _height(1) {};

		Box(double width, double length, double height) : _width(width), _length(length), _height(height){};
	
		virtual void build(std::shared_ptr<DRHI::DynamicRHI> rhi, DRHI::DynamicCommandPool* commandPool, DRHI::DynamicImage shadowImage, DRHI::DynamicImageView shadowImageView, DRHI::DynamicSampler shadowSampler)
		{
			// create vertices
			Vertex v1{},v2{},v3{},v4{},v5{},v6{},v7{},v8{};
			v1.pos = Vector3(-_width / 2, -_length / 2, _height / 2);
			v2.pos = Vector3(_width / 2, -_length / 2, _height / 2);
			v3.pos = Vector3(_width / 2, _length / 2, _height / 2);
			v4.pos = Vector3(-_width / 2, _length / 2, _height / 2);
			v5.pos = Vector3(-_width / 2, -_length / 2, -_height / 2);
			v6.pos = Vector3(_width / 2, -_length / 2, -_height / 2);
			v7.pos = Vector3(_width / 2, _length / 2, -_height / 2);
			v8.pos = Vector3(-_width / 2, _length / 2, -_height / 2);

			_vertices = { v1,v2,v3,v4,v5,v6,v7,v8 };

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

		void clean(std::shared_ptr<DRHI::DynamicRHI> rhi)
		{
			rhi->clearBuffer(&_vertexBuffer, &_vertexDeviceMemory);
			rhi->clearBuffer(&_indexBuffer, &_indexDeviceMemory);
		}
	};
}