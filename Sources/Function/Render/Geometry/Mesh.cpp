#include"Mesh.h"

namespace FOCUS
{
	void Mesh::build(std::shared_ptr<DRHI::DynamicRHI> rhi, DRHI::DynamicCommandPool* commandPool)
	{
        auto api = rhi->getCurrentAPI();
        auto bufferUsage = DRHI::DynamicBufferUsageFlags(api);
        auto memoryFlags = DRHI::DynamicMemoryPropertyFlagBits(api);

		//create vertex buffer
		auto vertexBufferSize = sizeof(_vertices[0]) * _vertices.size();
		rhi->createDynamicBuffer(&_vertexBuffer, &_vertexDeviceMemory, commandPool, vertexBufferSize, _vertices.data(), bufferUsage.BUFFER_USAGE_VERTEX_BUFFER_BIT, memoryFlags.MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		//create index buffer
		auto indexBufferSize = sizeof(_indices[0]) * _indices.size();
		rhi->createDynamicBuffer(&_indexBuffer, &_indexDeviceMemory, commandPool, indexBufferSize, _indices.data(), bufferUsage.BUFFER_USAGE_INDEX_BUFFER_BIT, memoryFlags.MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        _material->build(rhi, commandPool);
    }

    void Mesh::draw(std::shared_ptr<DRHI::DynamicRHI> rhi, DRHI::DynamicCommandBuffer* commandBuffer)
    {
        auto api = rhi->getCurrentAPI();
        auto indexType = DRHI::DynamicIndexType(api);

        _material->draw(rhi, commandBuffer);

        rhi->bindVertexBuffers(&_vertexBuffer, commandBuffer);
        rhi->bindIndexBuffer(&_indexBuffer, commandBuffer, indexType.INDEX_TYPE_UINT32);

        //draw model
        rhi->drawIndexed(commandBuffer, static_cast<uint32_t>(_indices.size()), 1, 0, 0, 0);
    }

    void Mesh::updateUniformBuffer(UniformUpdateData uud)
    {
        _material->updateUniformBuffer(uud);
    }
}