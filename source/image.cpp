#include "image.hpp"
#include "ray.hpp"

namespace engine {

Image generate_image(const Scene& scene) {
    Image result{};
    result.resize(scene.height * scene.width * 3);

    std::uint32_t W = scene.width;
    std::uint32_t H = scene.height;
    for (size_t i = 0; i < H; ++i) {
        for (size_t j = 0; j < W; ++j) {
            ray::Ray ray = ray::generate_ray(scene, {j, i});
            const auto& [_, color] = ray::raytrace(ray, scene);

            size_t index = (i * W + j) * 3;
            result[index] = color.at(0);
            result[index + 1] = color.at(1);
            result[index + 2] = color.at(2);
        }
    }
    return result;
}

} // namespace engine
