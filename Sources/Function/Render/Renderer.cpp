#include"Renderer.h"
#include"RenderResources.h"
#include"Mesh.h"

namespace FOCUS
{
	Renderer::Renderer(DRHI::API api, DRHI::PlatformInfo platformCI)
	{
		switch (api)
		{
		default:
		case DRHI::VULKAN:
			DRHI::RHICreateInfo rhiCI{};
			rhiCI.platformInfo = platformCI;
			_rhiContext = new DRHI::VulkanDRHI(rhiCI);
			break;
		}
	}

	void Renderer::initialize()
	{
		_rhiContext->initialize();

		DRHI::PipelineCreateInfo pci = {};
		pci.vertexShader = "../../../Shaders/model_vertex.spv";
		pci.fragmentShader = "../../../Shaders/model_fragment.spv";

		dynamic_cast<DRHI::VulkanDRHI*>(_rhiContext)->createPipeline(pci);
		_rhiContext->prepareCommandBuffer();

		RenderResources res;
		const char* modelPath = "../../../Asset/Models/viking_room.obj";
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		res.loadModel(modelPath, &vertices, &indices);
	}

	void Renderer::draw()
	{
		_rhiContext->frameOnTick();
	}

	void Renderer::clean()
	{
		_rhiContext->clean();
	}
}