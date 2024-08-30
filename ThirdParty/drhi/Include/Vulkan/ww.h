#include"../InterfaceType.h"
#include<stdexcept>

class e
{
public:
	void c()
	{
		check();
	}

	void check()
	{
		if (glfwVulkanSupported() == GLFW_FALSE)
		{
			throw std::runtime_error("failed to create window surface!");
		}
	}
};
