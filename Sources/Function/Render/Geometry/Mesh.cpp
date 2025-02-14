#include"Mesh.h"

namespace FOCUS
{
	void Mesh::build(std::shared_ptr<DRHI::DynamicRHI> rhi, DRHI::DynamicCommandPool* commandPool, DRHI::DynamicImage shadowImage, DRHI::DynamicImageView shadowImageView, DRHI::DynamicSampler shadowSampler)
	{
        if((_vertices.size() == 0) || (_indices.size() == 0)) return;

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

        //build material
        _material->build(rhi, commandPool, shadowImage, shadowImageView, shadowSampler);
    }

    void Mesh::draw(std::shared_ptr<DRHI::DynamicRHI> rhi, DRHI::DynamicCommandBuffer* commandBuffer, bool isShadowPass)
    {
        if (!_indexBuffer.valid() || !_vertexBuffer.valid()) return;

        auto api = rhi->getCurrentAPI();
        auto indexType = DRHI::DynamicIndexType(api);

        if(isShadowPass)
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

    void Mesh::draw(std::shared_ptr<DRHI::DynamicRHI> rhi, DRHI::DynamicCommandBuffer* commandBuffer, DRHI::DynamicPipeline pipeline, DRHI::DynamicPipelineLayout pipelineLayout, DRHI::DynamicDescriptorSet set)
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

    void Mesh::updateUniformBuffer(UniformUpdateData uud)
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
}