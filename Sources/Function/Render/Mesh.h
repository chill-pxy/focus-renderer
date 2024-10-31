#pragma once

#include<tiny_obj_loader.h>

#include<functional>
#include<memory>

#include"Materials/Material.h"
#include"RenderResource.h"
#include"../../Core/Math.h"

namespace FOCUS
{
	struct Vertex
	{
        Vector3 pos;
        Vector3 color;
        Vector2 texCoord;

        bool operator==(const Vertex& other) const 
        {
            return pos == other.pos && color == other.color && texCoord == other.texCoord;
        }
	};

    class Mesh : public RenderResource
    {
    public:
        tinyobj::attrib_t        _attrib;
        std::vector<Vertex>      _vertices;
        std::vector<uint32_t>    _indices;

        bool _needInit{ true };

    public:
        Mesh() = default;

        virtual void build(std::shared_ptr<DRHI::DynamicRHI> rhi);
        virtual void draw(uint32_t index, std::shared_ptr<DRHI::DynamicRHI> rhi);
       
        void updateUniformBuffer(uint32_t currentImage, Matrix4 view);
    };

    Mesh* loadModel(const char* modelPath);
}

namespace std
{
    template<> struct hash<FOCUS::Vertex> 
    {
        size_t operator()(FOCUS::Vertex const& vertex) const 
        {
            return ((hash<FOCUS::Vector3>()(vertex.pos) ^ (hash<FOCUS::Vector3>()(vertex.color) << 1)) >> 1) ^ (hash<FOCUS::Vector2>()(vertex.texCoord) << 1);
        }
    };
}