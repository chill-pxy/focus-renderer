import Engine;

#include <iostream>
#include <stdexcept>

int main()
{
    auto app = focus::EngineSingleton::getInstance();
    app->initialize();

    try {
        app->run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}