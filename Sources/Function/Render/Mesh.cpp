#include"Mesh.h"

namespace FOCUS
{
	void Mesh::build(std::shared_ptr<DRHI::DynamicRHI> rhi)
	{
        auto bufferUsage = DRHI::DynamicBufferUsageFlags(rhi->getCurrentAPI());
        auto format = DRHI::DynamicFormat(rhi->getCurrentAPI());
        auto descriptorType = DRHI::DynamicDescriptorType(rhi->getCurrentAPI());
        auto imageLayout = DRHI::DynamicImageLayout(rhi->getCurrentAPI());
        auto stageFlags = DRHI::DynamicShaderStageFlags(rhi->getCurrentAPI());

        rhi->createDescriptorPool(&_descriptorPool);

        std::vector<DRHI::DynamicDescriptorSetLayoutBinding> dsbs(2);
        dsbs[0].binding = 0;
        dsbs[0].descriptorCount = 1;
        dsbs[0].descriptorType = descriptorType.DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        dsbs[0].pImmutableSamplers = nullptr;
        dsbs[0].stageFlags = stageFlags.SHADER_STAGE_VERTEX_BIT;

        dsbs[1].binding = 1;
        dsbs[1].descriptorCount = 1;
        dsbs[1].descriptorType = descriptorType.DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        dsbs[1].pImmutableSamplers = nullptr;
        dsbs[1].stageFlags = stageFlags.SHADER_STAGE_FRAGMENT_BIT;

        rhi->createDescriptorSetLayout(&_descriptorSetLayout, &dsbs);

		//create vertex buffer
		auto vertexBufferSize = sizeof(_vertices[0]) * _vertices.size();
		rhi->createDynamicBuffer(&_vertexBuffer, &_vertexDeviceMemory, vertexBufferSize, _vertices.data(), bufferUsage.BUFFER_USAGE_VERTEX_BUFFER_BIT);

		//create index buffer
		auto indexBufferSize = sizeof(_indices[0]) * _indices.size();
		rhi->createDynamicBuffer(&_indexBuffer, &_indexDeviceMemory, indexBufferSize, _indices.data(), bufferUsage.BUFFER_USAGE_INDEX_BUFFER_BIT);

        //create uniform buffer
        rhi->createUniformBuffer(&_uniformBuffers, &_uniformBuffersMemory, &_uniformBuffersMapped, sizeof(UniformBufferObject));
	
        //binding sampler and image view
        rhi->createTextureImage(&_textureImage, &_textureMemory, _texture->_width, _texture->_height, _texture->_channels, _texture->_pixels);
        rhi->createImageView(&_textureImageView, &_textureImage, format.FORMAT_R8G8B8A8_SRGB);
        rhi->createTextureSampler(&_textureSampler);

        for (int i = 0; i < _uniformBuffers.size(); ++i)
        {
            DRHI::DynamicDescriptorBufferInfo bufferInfo;
            bufferInfo.set(rhi->getCurrentAPI(), _uniformBuffers[i], sizeof(UniformBufferObject));
            _descriptorBufferInfos.push_back(bufferInfo);
        }

        std::vector<DRHI::DynamicWriteDescriptorSet> wds(2);
        wds[0].descriptorType = descriptorType.DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        wds[0].dstBinding = 0;
        wds[0].pBufferInfo = &_descriptorBufferInfos[0];
        wds[0].descriptorCount = 1;
        
        DRHI::DynamicDescriptorImageInfo dii{};
        dii.imageLayout = imageLayout.IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        dii.imageView = _textureImageView;
        dii.sampler = _textureSampler;

        wds[1].descriptorType = descriptorType.DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        wds[1].dstBinding = 1;
        wds[1].pBufferInfo = &_descriptorBufferInfos[1];
        wds[1].descriptorCount = 1;
        wds[1].pImageInfo = &dii;

        rhi->createDescriptorSet(&_descriptorSet, &_descriptorSetLayout, &_descriptorPool, &wds);
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
}