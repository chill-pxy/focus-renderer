#include"Mesh.h"

namespace focus
{
	void Mesh::build(std::shared_ptr<drhi::DynamicRHI> rhi, drhi::DynamicCommandPool* commandPool)
	{
        if((_vertices.size() == 0) || (_indices.size() == 0)) return;

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

        //build material
        _material->build(rhi, commandPool);
    }

    void Mesh::draw(std::shared_ptr<drhi::DynamicRHI> rhi, drhi::DynamicCommandBuffer* commandBuffer, RenderResourcePipeline pipeline)
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

    void Mesh::draw(std::shared_ptr<drhi::DynamicRHI> rhi, drhi::DynamicCommandBuffer* commandBuffer, drhi::DynamicPipeline pipeline, drhi::DynamicPipelineLayout pipelineLayout, drhi::DynamicDescriptorSet set)
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

    void Mesh::updateUniformBuffer(UniformUpdateData uud)
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
}