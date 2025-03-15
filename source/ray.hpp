#pragma once

#include "glm/ext/vector_float3.hpp"
#include "scene.hpp"

#include <array>
#include <cstdint>
#include <optional>
#include <utility>

namespace engine::ray {

using Color = std::array<std::uint8_t, 3>;

struct Ray {
    Ray() {
        start = {0.f, 0.f, 0.f};
        direction = {0.f, 0.f, 0.f};
    }

    glm::vec3 start;
    glm::vec3 direction;
};

Ray generate_ray(const Scene& a_scene, std::pair<std::uint32_t, std::uint32_t> a_pixel_coord);
std::optional<float> intersection(Ray& a_ray, const Plane& a_plane);
std::optional<float> intersection(Ray& a_ray, const Ellipsoid& a_sphere);
std::optional<float> intersection(Ray& a_ray, const Box& a_box);
std::optional<float> intersection(Ray& a_ray, const Shape& a_object);
std::pair<std::optional<float>, Color> raytrace(Ray& a_ray, const Scene& a_scene);

} // namespace engine::ray
