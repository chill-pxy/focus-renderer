#pragma once

#include<tiny_obj_loader.h>

#include<functional>
#include<memory>

#include"../Materials/Material.h"
#include"../RenderResource.h"
#include"MeshVertex.h"


namespace FOCUS
{
    class Mesh : public RenderResource
    {
    public:
        tinyobj::attrib_t        _attrib{};
        std::vector<Vertex>      _vertices{};
        std::vector<uint32_t>    _indices{};

    public:
        Mesh() = default;

        virtual void build(std::shared_ptr<DRHI::DynamicRHI> rhi, DRHI::DynamicCommandPool* commandPool);
        virtual void draw(std::shared_ptr<DRHI::DynamicRHI> rhi, DRHI::DynamicCommandBuffer* commandBuffer);
        virtual void updateUniformBuffer(UniformUpdateData uud);
    };

    std::shared_ptr<Mesh> loadModel(std::string modelPath);
}