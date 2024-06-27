//import window_context;
import vulkan_rhi;

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <limits>
#include <optional>
#include <set>

int main()
{
	/*auto window = new FOCUS::WindowContext();
	window->init();

	auto vulkan = new FOCUS::VulkanRHI();
	vulkan->init(window->getWindowInstance());

	return 0;*/

    HelloTriangleApplication app;

    try {
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}