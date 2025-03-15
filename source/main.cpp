#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include "io.hpp"

static bool verbose = false;

int main(int argc, char* argv[]) {
    if (argc != 3 && argc != 4) {
        std::cout << "Usage: ./engine <path-to-scene> <path-to-image> [-v]\n";
        return EXIT_FAILURE;
    }
    if (argc == 4 && std::string(argv[3]) != "-v") {
        std::cout << "Unknown argument: " << argv[3] << '\n';
        return EXIT_FAILURE;
    }
    else if (argc == 4 && std::string(argv[3]) == "-v") {
        verbose = true;
    }

    try {
        engine::Scene scene = engine::io::load_scene(std::string(argv[1]));
        engine::Image image = engine::generate_image(scene);
        engine::io::write_image(std::string(argv[2]), scene.width, scene.height, image);
    
        if (verbose) {
            std::cout << scene << '\n';
        }
    }
    catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
