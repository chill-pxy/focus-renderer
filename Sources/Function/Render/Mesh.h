#pragma once

#include<tiny_obj_loader.h>

#include<functional>
#include<memory>

#include"Materials/Material.h"
#include"RenderResource.h"
#include"MeshVertex.h"

namespace FOCUS
{
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
       
        void updateUniformBuffer(uint32_t currentImage, std::shared_ptr<RenderCamera> camera);
    };

    Mesh* loadModel(const char* modelPath);
}