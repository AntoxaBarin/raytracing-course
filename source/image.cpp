#include "image.hpp"
#include "ray.hpp"

Image generate_image(const Scene& a_scene) {
    Image result{};
    result.resize(a_scene.height * a_scene.width * 3);

    std::uint32_t W = a_scene.width;
    std::uint32_t H = a_scene.height;
    for (size_t i = 0; i < H; ++i) {
        for (size_t j = 0; j < W; ++j) {
            ray::Ray ray = ray::generate_ray(a_scene, {j, i});
            const auto& [_, color] = ray::raytrace(ray, a_scene);
            
            size_t index = (i * W + j) * 3;
            result[index] = color.at(0);
            result[index + 1] = color.at(1);
            result[index + 2] = color.at(2); 
        }
    }
    return result;
}
