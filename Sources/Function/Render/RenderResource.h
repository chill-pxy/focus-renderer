#pragma once

#include<stdexcept>
#include<unordered_map>

#include<tiny_obj_loader.h>
#include<stb_image.h>
#include<drhi.h>

namespace FOCUS
{
    class RenderResource
    {
    public:
        DRHI::DynamicBuffer       _vertexBuffer;
        DRHI::DynamicDeviceMemory _vertexDeviceMemory;
        
        DRHI::DynamicBuffer       _indexBuffer;   
        DRHI::DynamicDeviceMemory _indexDeviceMemory;

        std::vector<void*>                     _uniformBuffersMapped;
        std::vector<DRHI::DynamicBuffer>       _uniformBuffers;
        std::vector<DRHI::DynamicDeviceMemory> _uniformBuffersMemory;

        DRHI::DynamicImage        _textureImage;
        DRHI::DynamicImageView    _textureImageView;
        DRHI::DynamicSampler      _textureSampler;
        DRHI::DynamicDeviceMemory _textureMemory;
        
        std::vector<DRHI::DynamicDescriptorBufferInfo> _descriptorBufferInfos;
        DRHI::DynamicDescriptorPool                    _descriptorPool;
        DRHI::DynamicDescriptorSet                     _descriptorSet;
        DRHI::DynamicDescriptorSetLayout               _descriptorSetLayout;

    public:
        RenderResource() = default;
        virtual void build(std::shared_ptr<DRHI::DynamicRHI> rhi) = 0;
        virtual void draw(uint32_t index, std::shared_ptr<DRHI::DynamicRHI> rhi) = 0;
    };
}