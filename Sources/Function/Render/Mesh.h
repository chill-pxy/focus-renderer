#pragma once

#include<tiny_obj_loader.h>

#include<functional>
#include<memory>

#include"RenderResource.h"
#include"Texture.h"
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

    struct UniformBufferObject 
    {
        alignas(16) Matrix4 model;
        alignas(16) Matrix4 view;
        alignas(16) Matrix4 proj;
    };

    class Mesh : public RenderResource
    {
    public:
        tinyobj::attrib_t        _attrib;
        std::vector<Vertex>      _vertices;
        std::vector<uint32_t>    _indices;
        std::shared_ptr<Texture> _texture;

        bool _needInit{ true };

    private:
        DRHI::DynamicPipeline _pipeline;
        DRHI::DynamicPipelineLayout _pipelineLayout;

    public:
        Mesh() = default;

        virtual void build(std::shared_ptr<DRHI::DynamicRHI> rhi);
        virtual void draw(uint32_t index, std::shared_ptr<DRHI::DynamicRHI> rhi);
       
        void updateUniformBuffer(uint32_t currentImage, Matrix4 view);
        void preparePipeline(std::shared_ptr<DRHI::DynamicRHI> rhi);
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