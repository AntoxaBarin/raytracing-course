#pragma once

#include "glm/ext/vector_float3.hpp"
#include "primitive.hpp"
#include "scene.hpp"

#include <array>
#include <cstdint>
#include <optional>
#include <utility>

namespace engine::ray {

using Color = std::array<std::uint8_t, 3>;

struct Ray {
    Ray()
        : start({0.f, 0.f, 0.f}),
          direction({0.f, 0.f, 0.f}) {}

    glm::vec3 start;
    glm::vec3 direction;
};

Ray generate_ray(const Scene& scene, std::pair<std::uint32_t, std::uint32_t> pixel_coord);
std::optional<Intersection> intersection(Ray& ray, Plane* plane);
std::optional<Intersection> intersection(Ray& ray, Ellipsoid* sphere);
std::optional<Intersection> intersection(Ray& ray, Box* box);
std::optional<Intersection> intersection(Ray ray, Shape* object);
std::pair<std::optional<float>, glm::vec3> raytrace(Ray& ray, const Scene& scene, std::uint32_t ray_depth);

bool is_shadowed(const Scene& scene, const Ray& ray, float dist);
glm::vec3 calc_color(const Scene& scene, Shape* obj, Ray ray, const Intersection& inter, std::uint32_t ray_depth);
glm::vec3 calc_diffuse_rawcolor(const Scene& scene, Shape* obj, Ray ray, const Intersection& inter, std::uint32_t ray_depth);
glm::vec3 calc_metallic_rawcolor(const Scene& scene, Shape* obj, Ray ray, const Intersection& inter, std::uint32_t ray_depth);
glm::vec3 calc_dielectric_rawcolor(const Scene& scene, Shape* obj, Ray ray, const Intersection& inter, std::uint32_t ray_depth);

} // namespace engine::ray
