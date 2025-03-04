#include "ray.hpp"

#include <optional>

namespace ray {

Ray generate_ray(const Camera& a_camera, const std::pair<std::uint32_t, std::uint32_t> a_pixel_coord) {     
    return Ray{}; 
}

std::optional<float> intersection(const Ray& a_ray, const Plane& a_plane) { 
    return {};
}

std::optional<float> intersection(const Ray& a_ray, const Ellipsoid& a_sphere) {
    return {};
}

std::optional<float> intersection(const Ray& a_ray, const Box& a_box) {
    return {};
}

std::optional<float> intersection(const Ray& a_ray, const Shape& a_object)  {
    return {};
}

std::optional<std::pair<float, Color>> intersection(const Ray& a_ray, const Scene& a_scene)  {
    return {};
}

Color raytrace(const Scene& a_scene, const Ray& a_ray)  {
    return {0, 0, 0};
}


} // namespace ray
