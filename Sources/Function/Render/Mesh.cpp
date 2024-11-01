#include"Mesh.h"

namespace FOCUS
{
	void Mesh::build(std::shared_ptr<DRHI::DynamicRHI> rhi)
	{
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

    Mesh* loadModel(const char* modelPath)
    {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, modelPath))
        {
            throw std::runtime_error(warn + err);
        }

        std::unordered_map<Vertex, uint32_t> uniqueVertices{};

        for (const auto& shape : shapes)
        {
            for (const auto& index : shape.mesh.indices)
            {
                Vertex vertex{};

                vertex.pos =
                {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                };

                vertex.texCoord =
                {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
                };

                vertex.normal =
                {
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2]
                };

                vertex.color = { 1.0f, 1.0f, 1.0f };

                if (uniqueVertices.count(vertex) == 0)
                {
                    uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                    vertices.push_back(vertex);
                }

                indices.push_back(uniqueVertices[vertex]);
            }
        }

        Mesh* mesh = new Mesh();
        mesh->_attrib = attrib;
        mesh->_indices = indices;
        mesh->_vertices = vertices;

        return mesh;
    }

    void Mesh::draw(uint32_t index, std::shared_ptr<DRHI::DynamicRHI> rhi)
    {
        auto api = rhi->getCurrentAPI();
        auto indexType = DRHI::DynamicIndexType(api);

        _material->draw(index, rhi);

        rhi->bindVertexBuffers(&_vertexBuffer, index);
        rhi->bindIndexBuffer(&_indexBuffer, index, indexType.INDEX_TYPE_UINT32);

        //draw model
        rhi->drawIndexed(index, static_cast<uint32_t>(_indices.size()), 1, 0, 0, 0);
    }

    void Mesh::updateUniformBuffer(uint32_t currentImage, std::shared_ptr<RenderCamera> camera)
    {
        _material->updateUniformBuffer(currentImage, camera);
    }
}