#pragma once

#include<vector>

#include"../RenderResource.h"
#include"MeshVertex.h"

namespace focus
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
	
		virtual void build(std::shared_ptr<drhi::DynamicRHI> rhi, drhi::DynamicCommandPool* commandPool)
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

			v1.normal = normalize(Vector3(0, 0, 1)); 
			v2.normal = normalize(Vector3(0, 0, 1));
			v3.normal = normalize(Vector3(0, 0, 1));
			v4.normal = normalize(Vector3(0, 0, 1));

			v5.normal = normalize(Vector3(0, 0, -1));
			v6.normal = normalize(Vector3(0, 0, -1));
			v7.normal = normalize(Vector3(0, 0, -1));
			v8.normal = normalize(Vector3(0, 0, -1));

			v1.texCoord = Vector2(0, 0);
			v2.texCoord = Vector2(1, 0); 
			v3.texCoord = Vector2(1, 1); 
			v4.texCoord = Vector2(0, 1); 

			v5.texCoord = Vector2(0, 0); 
			v6.texCoord = Vector2(1, 0);
			v7.texCoord = Vector2(1, 1);
			v8.texCoord = Vector2(0, 1); 

			_vertices = { v1,v2,v3, v4, v5, v6, v7, v8 };

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
			auto bufferUsage = drhi::DynamicBufferUsageFlags(api);
			auto memoryFlags = drhi::DynamicMemoryPropertyFlagBits(api);

			//create vertex buffer
			auto vertexBufferSize = sizeof(_vertices[0]) * _vertices.size();
			rhi->createDynamicBuffer(&_vertexBuffer, &_vertexDeviceMemory, commandPool, vertexBufferSize, _vertices.data(), bufferUsage.BUFFER_USAGE_VERTEX_BUFFER_BIT, memoryFlags.MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

			//create index buffer
			auto indexBufferSize = sizeof(_indices[0]) * _indices.size();
			rhi->createDynamicBuffer(&_indexBuffer, &_indexDeviceMemory, commandPool, indexBufferSize, _indices.data(), bufferUsage.BUFFER_USAGE_INDEX_BUFFER_BIT, memoryFlags.MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

			//initiailize shadow map
			_shadow = std::make_shared<ShadowMap>();
			_shadow->initialize(rhi);

			//initialize deffered
			_deffered = std::make_shared<DefferedPipeline>();
			_deffered->initialize(rhi);

			_material->build(rhi, commandPool);
		}

		virtual void draw(std::shared_ptr<drhi::DynamicRHI> rhi, drhi::DynamicCommandBuffer* commandBuffer, RenderResourcePipeline pipeline)
		{
			if (!_indexBuffer.valid() || !_vertexBuffer.valid()) return;

			auto api = rhi->getCurrentAPI();
			auto indexType = drhi::DynamicIndexType(api);

			switch (pipeline)
			{
			case RenderResourcePipeline::SCENE:
				_material->draw(rhi, commandBuffer);
				break;
			case RenderResourcePipeline::SHADOW:
				_shadow->draw(rhi, commandBuffer);
				break;
			case RenderResourcePipeline::DEFFERED:
				_deffered->draw(rhi, commandBuffer);
				break;
			}

			rhi->bindVertexBuffers(&_vertexBuffer, commandBuffer);
			rhi->bindIndexBuffer(&_indexBuffer, commandBuffer, indexType.INDEX_TYPE_UINT32);

			//draw model
			rhi->drawIndexed(commandBuffer, static_cast<uint32_t>(_indices.size()), 1, 0, 0, 0);
		}

		virtual void draw(std::shared_ptr<drhi::DynamicRHI> rhi, drhi::DynamicCommandBuffer* commandBuffer, drhi::DynamicPipeline pipeline, drhi::DynamicPipelineLayout pipelineLayout, drhi::DynamicDescriptorSet set)
		{
			auto api = rhi->getCurrentAPI();
			auto indexType = drhi::DynamicIndexType(api);
			auto bindPoint = drhi::DynamicPipelineBindPoint(api);

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

			_deffered->updateUniformBuffer(uud);
			_shadow->updateUniform(uud);
			_material->updateUniformBuffer(uud);
		}

		void clean(std::shared_ptr<drhi::DynamicRHI> rhi)
		{
			rhi->clearBuffer(&_vertexBuffer, &_vertexDeviceMemory);
			rhi->clearBuffer(&_indexBuffer, &_indexDeviceMemory);
		}
	};
}