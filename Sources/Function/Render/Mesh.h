#pragma once
#include<glm/glm.hpp>
#include<glm/gtx/hash.hpp>
#include<tiny_obj_loader.h>

#include<functional>
#include<memory>

#include"RenderResource.h"
#include"Texture.h"

namespace FOCUS
{
	struct Vertex
	{
        glm::vec3 pos;
        glm::vec3 color;
        glm::vec2 texCoord;

        bool operator==(const Vertex& other) const 
        {
            return pos == other.pos && color == other.color && texCoord == other.texCoord;
        }
	};

    struct UniformBufferObject 
    {
        alignas(16) glm::mat4 model;
        alignas(16) glm::mat4 view;
        alignas(16) glm::mat4 proj;
    };

    class Mesh : public RenderResource
    {
    public:
        tinyobj::attrib_t        _attrib;
        std::vector<Vertex>      _vertices;
        std::vector<uint32_t>    _indices;
        std::shared_ptr<Texture> _texture;

    public:
        Mesh() = default;

        virtual void build(std::shared_ptr<DRHI::DynamicRHI> rhi);
    };

    Mesh* loadModel(const char* modelPath);
}

namespace std
{
    template<> struct hash<FOCUS::Vertex> 
    {
        size_t operator()(FOCUS::Vertex const& vertex) const 
        {
            return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
        }
    };
}