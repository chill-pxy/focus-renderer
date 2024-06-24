import window_context;
import vulkan_rhi;

int main()
{
	auto window = new FOCUS::WindowContext();
	window->init();

	auto vulkan = new FOCUS::VulkanRHI();
	vulkan->init(window->getWindowInstance());

	return 0;
}