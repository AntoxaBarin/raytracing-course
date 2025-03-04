#pragma once

#include "glm/ext/vector_float3.hpp"
#include "scene.hpp"

#include <array>
#include <cstdint>
#include <optional>
#include <utility>

namespace ray {

using Color = std::array<std::uint8_t, 3>;

struct Ray {
    glm::vec3 start;
    glm::vec3 direction;
};

Ray generate_ray(const Camera& a_camera, const std::pair<std::uint32_t, std::uint32_t> a_pixel_coord);
std::optional<float> intersection(const Ray& a_ray, const Plane& a_plane);
std::optional<float> intersection(const Ray& a_ray, const Ellipsoid& a_sphere);
std::optional<float> intersection(const Ray& a_ray, const Box& a_box);
std::optional<float> intersection(const Ray& a_ray, const Shape& a_object);
std::optional<std::pair<float, Color>> intersection(const Ray& a_ray, const Scene& a_scene);
Color raytrace(const Scene& a_scene, const Ray& a_ray);

} // namespace ray
