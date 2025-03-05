#include "ray.hpp"
#include "glm/geometric.hpp"
#include "primitive.hpp"

#include <cmath>
#include <optional>
#include <stdexcept>

namespace ray {

uint8_t color_converter(float value) { return static_cast<uint8_t>(std::round(value * 255.0f)); }

Ray generate_ray(const Scene& a_scene, std::pair<std::uint32_t, std::uint32_t> a_pixel_coord)
{
    Ray ray{};
    ray.start = a_scene.camera.camera_position;

    float x = static_cast<float>(a_pixel_coord.first) + 0.5;
    float y = static_cast<float>(a_pixel_coord.second) + 0.5;

    x = (2.f * x / static_cast<float>(a_scene.width) - 1) * tanf(a_scene.camera.camera_fov_x / 2);
    y = -(2.f * y / static_cast<float>(a_scene.height) - 1) *
        (tanf(a_scene.camera.camera_fov_x / 2) / (static_cast<float>(a_scene.width) / a_scene.height));
    ray.direction = glm::normalize(x * a_scene.camera.camera_right + y * a_scene.camera.camera_up + a_scene.camera.camera_forward);
    return ray;
}

std::optional<float> intersection(Ray& a_ray, Plane* a_plane)
{
    float t = -glm::dot(a_ray.start, a_plane->normal) / glm::dot(a_ray.direction, a_plane->normal);
    if (t < 0) {
        return {};
    }
    return t;
}

std::optional<float> intersection(Ray& r, Ellipsoid* e)
{
    float a = glm::dot(glm::vec3{r.direction.x / e->radius.x, r.direction.y / e->radius.y, r.direction.z / e->radius.z},
                       glm::vec3{r.direction.x / e->radius.x, r.direction.y / e->radius.y, r.direction.z / e->radius.z});
    float b = glm::dot(glm::vec3{r.start.x / e->radius.x, r.start.y / e->radius.y, r.start.z / e->radius.z},
                       glm::vec3{r.direction.x / e->radius.x, r.direction.y / e->radius.y, r.direction.z / e->radius.z});
    float c = glm::dot(glm::vec3{r.start.x / e->radius.x, r.start.y / e->radius.y, r.start.z / e->radius.z},
                       glm::vec3{r.start.x / e->radius.x, r.start.y / e->radius.y, r.start.z / e->radius.z});
    --c;

    float D = b * b - a * c;
    if (D < 0) {
        return {};
    }
    float t_1 = (-b - sqrt(D)) / a;
    float t_2 = (-b + sqrt(D)) / a;
    if (t_1 < 0) {
        if (t_2 < 0) {
            return {};
        }
        else {
            return t_2;
        }
    }
    return t_1;
}

std::optional<float> intersection(Ray& a_ray, Box* a_box)
{
    float tx_1 = (a_box->size.x - a_ray.start.x) / a_ray.direction.x;
    float tx_2 = (-a_box->size.x - a_ray.start.x) / a_ray.direction.x;
    if (tx_2 < tx_1) {
        std::swap(tx_1, tx_2);
    }
    float ty_1 = (a_box->size.y - a_ray.start.y) / a_ray.direction.y;
    float ty_2 = (-a_box->size.y - a_ray.start.y) / a_ray.direction.y;
    if (ty_2 < ty_1) {
        std::swap(ty_1, ty_2);
    }
    float tz_1 = (a_box->size.z - a_ray.start.z) / a_ray.direction.z;
    float tz_2 = (-a_box->size.z - a_ray.start.z) / a_ray.direction.z;
    if (tz_2 < tz_1) {
        std::swap(tz_1, tz_2);
    }

    float t_1 = std::max(tx_1, std::max(ty_1, tz_1));
    float t_2 = std::min(tx_2, std::min(ty_2, tz_2));
    if (t_1 > t_2) {
        return {};
    }
    else if (t_2 < 0) {
        return {};
    }
    else if (t_1 < 0) {
        return t_2;
    }
    return t_1;
}

std::optional<float> intersection(Ray a_ray, Shape* a_object)
{
    a_ray.start -= a_object->position;
    glm::quat reversed_rotation = glm::inverse(a_object->rotation);
    a_ray.start = reversed_rotation * a_ray.start;
    a_ray.direction = glm::normalize(reversed_rotation * a_ray.direction);

    switch (a_object->type) {
    case PRIMITIVE_TYPE::Plane:
        return intersection(a_ray, dynamic_cast<Plane*>(a_object));
    case PRIMITIVE_TYPE::Ellipsoid:
        return intersection(a_ray, dynamic_cast<Ellipsoid*>(a_object));
    case PRIMITIVE_TYPE::Box:
        return intersection(a_ray, dynamic_cast<Box*>(a_object));
    default:
        throw std::runtime_error("Unknown primitive type");
    }
}

std::pair<std::optional<float>, Color> raytrace(Ray& a_ray, const Scene& a_scene)
{
    Color color = {color_converter(a_scene.bg_color.x), color_converter(a_scene.bg_color.y), color_converter(a_scene.bg_color.z)};
    std::optional<float> intersection_t = std::nullopt;

    for (auto primitive : a_scene.primitives) {
        auto intersection_result = intersection(a_ray, primitive);
        if (!intersection_result.has_value()) {
            continue;
        }
        if (!intersection_t.has_value() || intersection_t.value() > intersection_result.value()) {
            intersection_t = intersection_result;
            color = {color_converter(primitive->color.x), color_converter(primitive->color.y), color_converter(primitive->color.z)};
        }
    }
    return {intersection_t, color};
}

} // namespace ray
