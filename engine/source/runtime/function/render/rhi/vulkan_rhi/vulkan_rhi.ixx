module;

export module vulkan_rhi;

import rhi;

namespace FOCUS
{
    export class VulkanRHI final : public RHI
    {
    public:
        virtual void init() override final;
    
    private:
        void createInstance();
    };
}