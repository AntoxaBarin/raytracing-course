#include <iostream>

#include "image.hpp"
#include "io.hpp"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: ./engine <path-to-scene> <path-to-image>\n";
        return 1;
    }

    Scene scene = io::load_scene(argv[1]);
    std::cout << scene;
    Image image = generate_image(scene);
    io::write_image(argv[2], scene.width, scene.height, image);
}
