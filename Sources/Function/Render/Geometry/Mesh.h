#pragma once

#include<tiny_obj_loader.h>

#include<functional>
#include<memory>

#include"../Materials/Material.h"
#include"../RenderResource.h"
#include"MeshVertex.h"


namespace focus
{
    class Mesh : public RenderResource
    {
    public:
        std::vector<Vertex>      _vertices{};
        std::vector<uint32_t>    _indices{};

    public:
        Mesh() = default;

        virtual void build(std::shared_ptr<drhi::DynamicRHI> rhi, drhi::DynamicCommandPool* commandPool);
        virtual void draw(std::shared_ptr<drhi::DynamicRHI> rhi, drhi::DynamicCommandBuffer* commandBuffer, RenderResourcePipeline pipeline);
        virtual void draw(std::shared_ptr<drhi::DynamicRHI> rhi, drhi::DynamicCommandBuffer* commandBuffer, drhi::DynamicPipeline pipeline, drhi::DynamicPipelineLayout pipelineLayout, drhi::DynamicDescriptorSet set);
        virtual void updateUniformBuffer(UniformUpdateData uud);
    };
}