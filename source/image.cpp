#include "image.hpp"
#include "ray.hpp"

Image generate_image(const Scene& a_scene) {
    Image result{};
    result.reserve(a_scene.height * a_scene.width);

    std::uint32_t W = a_scene.width;
    std::uint32_t H = a_scene.height;
    for (size_t i = 0; i < W; ++i) {
        for (size_t j = 0; j < H; ++j) {
            ray::Ray ray = ray::generate_ray(a_scene.camera, {j, i});
            const auto& color = ray::raytrace(a_scene, ray);
            result[(i * W + j) * 3] = color.at(0);
            result[(i * W + j) * 3 + 1] = color.at(1);
            result[(i * W + j) * 3 + 2] = color.at(2); 
        }
    }
    return result;
}
