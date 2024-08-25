#include<drhi.h>

#include"MainPass.h"
#include"../Mesh.h"

namespace FOCUS
{
	void MainPass::initialize(DRHI::Context context)
	{
		//auto bindingDescription = Vertex::getBindingDescription();
		//auto attributeDescriptions = Vertex::getAttributeDescriptions();
		//
		//vertexInputInfo.vertexBindingDescriptionCount = 1;
		//vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		//vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
		//vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
        //static VkVertexInputBindingDescription getBindingDescription() {
        //    VkVertexInputBindingDescription bindingDescription{};
        //    bindingDescription.binding = 0;
        //    bindingDescription.stride = sizeof(Vertex);
        //    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        //    return bindingDescription;
        //}

        //static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
        //    std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

        //    attributeDescriptions[0].binding = 0;
        //    attributeDescriptions[0].location = 0;
        //    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        //    attributeDescriptions[0].offset = offsetof(Vertex, pos);

        //    attributeDescriptions[1].binding = 0;
        //    attributeDescriptions[1].location = 1;
        //    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        //    attributeDescriptions[1].offset = offsetof(Vertex, color);

        //    attributeDescriptions[2].binding = 0;
        //    attributeDescriptions[2].location = 2;
        //    attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        //    attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

        //    return attributeDescriptions;
        //}

        DRHI::VertexInputBindingDescription vertexInputBuildingDescription;
        vertexInputBuildingDescription.binding = 0;
        vertexInputBuildingDescription.stride = sizeof(Vertex);
        vertexInputBuildingDescription.inputRate = DRHI::VERTEX_INPUT_RATE_VERTEX;

        DRHI::VertexInputAttributeDescription vertexInputAttributeDescription[3];
        vertexInputAttributeDescription[0].binding = 0;
        vertexInputAttributeDescription[0].location = 0;
        vertexInputAttributeDescription[0].format = DRHI::FORMAT_R32G32_SFLOAT;
        vertexInputAttributeDescription[0].offset = offsetof(Vertex, Vertex::pos);

        vertexInputAttributeDescription[1].binding = 0;
        vertexInputAttributeDescription[1].location = 1;
        vertexInputAttributeDescription[1].format = DRHI::FORMAT_R32G32B32_SFLOAT;
        vertexInputAttributeDescription[1].offset = offsetof(Vertex, Vertex::color);

        vertexInputAttributeDescription[2].binding = 0;
        vertexInputAttributeDescription[2].location = 2;
        vertexInputAttributeDescription[2].format = DRHI::FORMAT_R32G32_SFLOAT;
        vertexInputAttributeDescription[2].offset = offsetof(Vertex, Vertex::texCoord);

        context.createRenderPass()
	}
}