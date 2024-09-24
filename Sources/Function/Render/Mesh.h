#pragma once
#include<glm/glm.hpp>
#include<glm/gtx/hash.hpp>
#include<tiny_obj_loader.h>

#include<functional>

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

    struct Mesh
    {
        tinyobj::attrib_t attrib;
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
    };

}

namespace std {
    template<> struct hash<FOCUS::Vertex> 
    {
        size_t operator()(FOCUS::Vertex const& vertex) const 
        {
            return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
        }
    };
}