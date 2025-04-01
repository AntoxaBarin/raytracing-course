#include "image.hpp"
#include "ray.hpp"
#include "utils.hpp"

namespace engine {

Image generate_image(const Scene& scene) {
    Image result{};
    result.resize(scene.height * scene.width * 3);

    std::uint32_t W = scene.width;
    std::uint32_t H = scene.height;
    for (size_t i = 0; i < H; ++i) {
        for (size_t j = 0; j < W; ++j) {
            glm::vec3 mean_color{};
            for (int k = 0; k < scene.samples; ++k) {
                ray::Ray ray = ray::generate_ray(scene, {j, i});
                const auto& [_, rawcolor] = ray::raytrace(ray, scene, 0);
                mean_color += rawcolor;
            }
            mean_color /= static_cast<float>(scene.samples);

            size_t index = (i * W + j) * 3;
            result[index] = color_converter(mean_color.r);
            result[index + 1] = color_converter(mean_color.g);
            result[index + 2] = color_converter(mean_color.b);
        }
    }
    return result;
}

} // namespace engine
