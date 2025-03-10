#include <iostream>
#include <stdexcept>

#include "io.hpp"

int main(int argc, char* argv[])
{
    if (argc != 3) {
        std::cout << "Usage: ./engine <path-to-scene> <path-to-image>\n";
        return 1;
    }

    try {
        engine::Scene scene = engine::io::load_scene(std::string(argv[1]));
        engine::Image image = engine::generate_image(scene);
        engine::io::write_image(std::string(argv[2]), scene.width, scene.height, image);
    }
    catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
