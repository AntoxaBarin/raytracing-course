#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include "io.hpp"

static bool verbose = false;
static bool multithreaded = false;

int main(int argc, char* argv[]) {
    if (argc < 3 || argc > 5) {
        std::cout << "Usage: ./engine <path-to-scene> <path-to-image> [-v] [-thread]\n";
        return EXIT_FAILURE;
    }
    if (argc > 3) {
        for (std::size_t i = 3; i < argc; ++i) {
            if (std::string(argv[i]) == "-v") {
                verbose = true;
            }
            else if (std::string(argv[i]) == "-thread") {
                multithreaded = true;
            }
            else {
                std::cout << "Unknown argument: " << argv[i] << '\n';
                return EXIT_FAILURE;
            }
        }
    }

    try {
        engine::Scene scene = engine::io::load_scene(std::string(argv[1]));
        if (verbose) {
            std::cout << scene << '\n';
        }
        engine::Image image = engine::generate_image(scene, multithreaded);
        engine::io::write_image(std::string(argv[2]), scene.width, scene.height, image);
    }
    catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
