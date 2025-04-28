#include "image.hpp"
#include "ray.hpp"
#include "utils.hpp"

#include <thread>

namespace engine {

Image generate_image_multithread(const Scene& scene);

Image generate_image(const Scene& scene, bool multithread) {
    if (multithread) {
        return generate_image_multithread(scene);
    }
    
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

Image generate_image_multithread(const Scene& scene) {
    Image result{};
    result.resize(scene.height * scene.width * 3);

    const std::size_t total_pixels = scene.height * scene.width;
    const std::size_t threads_num = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;
    threads.reserve(threads_num);

    const std::uint32_t W = scene.width;
    const std::uint32_t H = scene.height;

    const std::size_t base_chunk_size = total_pixels / threads_num;
    std::size_t offset = total_pixels % threads_num;

    auto worker = [&scene, W, &result](std::size_t begin, std::size_t end) {
        for (size_t i = begin; i < end; ++i) {
            const size_t row = i / W;
            const size_t col = i % W;

            glm::vec3 mean_color{};
            for (int k = 0; k < scene.samples; ++k) {
                ray::Ray ray = ray::generate_ray(scene, {col, row});
                const auto& [_, rawcolor] = ray::raytrace(ray, scene, 0);
                mean_color += rawcolor;
            }
            mean_color /= static_cast<float>(scene.samples);

            const size_t index = i * 3;
            result[index] = color_converter(mean_color.r);
            result[index + 1] = color_converter(mean_color.g);
            result[index + 2] = color_converter(mean_color.b);
        }
    };

    std::size_t start = 0;
    for (std::size_t i = 0; i < threads_num; ++i) {
        const std::size_t chunk_size = base_chunk_size + (i < offset ? 1 : 0);
        threads.emplace_back(worker, start, start + chunk_size);
        start += chunk_size;
    }

    for (auto& t : threads) {
        t.join();
    }

    return result;
}

} // namespace engine
