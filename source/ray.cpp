#include "ray.hpp"
#include "glm/geometric.hpp"
#include "primitive.hpp"
#include "utils.hpp"

#include <cmath>
#include <optional>
#include <stdexcept>

namespace engine::ray {

Ray generate_ray(const Scene& a_scene, std::pair<std::uint32_t, std::uint32_t> a_pixel_coord) {
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

std::optional<Intersection> intersection(Ray& a_ray, Plane* a_plane) {
    Intersection inter{};
    inter.t = -glm::dot(a_ray.start, a_plane->normal) / glm::dot(a_ray.direction, a_plane->normal);
    if (inter.t < 0) {
        return {};
    }
    inter.normal = a_plane->normal;
    if (glm::dot(a_ray.direction, a_plane->normal) >= 0) {
        inter.inside = true;
        // inter.normal *= 1;
    }
    return inter;
}

std::optional<Intersection> intersection(Ray& a_ray, Ellipsoid* a_ellips) {
    Intersection inter{};
    inter.normal = glm::vec3(1.0f);
    float a = glm::dot(
        glm::vec3{a_ray.direction.x / a_ellips->radius.x, a_ray.direction.y / a_ellips->radius.y, a_ray.direction.z / a_ellips->radius.z},
        glm::vec3{a_ray.direction.x / a_ellips->radius.x, a_ray.direction.y / a_ellips->radius.y, a_ray.direction.z / a_ellips->radius.z});

    float b = glm::dot(
        glm::vec3{a_ray.start.x / a_ellips->radius.x, a_ray.start.y / a_ellips->radius.y, a_ray.start.z / a_ellips->radius.z},
        glm::vec3{a_ray.direction.x / a_ellips->radius.x, a_ray.direction.y / a_ellips->radius.y, a_ray.direction.z / a_ellips->radius.z});

    float c =
        glm::dot(glm::vec3{a_ray.start.x / a_ellips->radius.x, a_ray.start.y / a_ellips->radius.y, a_ray.start.z / a_ellips->radius.z},
                 glm::vec3{a_ray.start.x / a_ellips->radius.x, a_ray.start.y / a_ellips->radius.y, a_ray.start.z / a_ellips->radius.z});
    --c;

    float D = b * b - a * c;
    if (D < 0) {
        return {};
    }
    float t_1 = (-b - sqrt(D)) / a;
    float t_2 = (-b + sqrt(D)) / a;
    inter.t = t_1;
    if (t_1 < 0) {
        if (t_2 < 0) {
            return {};
        }
        else {
            inter.t = t_2;
        }
    }

    glm::vec3 inter_point = a_ray.start + a_ray.direction * inter.t;
    glm::vec3 inter_norm = glm::vec3(inter_point.x / (a_ellips->radius.x * a_ellips->radius.x),
                                     inter_point.y / (a_ellips->radius.y * a_ellips->radius.y),
                                     inter_point.z / (a_ellips->radius.z * a_ellips->radius.z));
    inter.normal = glm::normalize(inter_norm);

    if (glm::dot(a_ray.direction, inter.normal) >= 0) {
        inter.inside = true;
        inter.normal *= -1;
    }
    return inter;
}

std::optional<Intersection> intersection(Ray& a_ray, Box* a_box) {
    Intersection inter{};
    inter.normal = glm::vec3(1.0f);

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
    inter.t = t_1;
    if (t_1 > t_2) {
        return {};
    }
    else if (t_2 < 0) {
        return {};
    }
    else if (t_1 < 0) {
        inter.t = t_2;
    }

    glm::vec3 inter_point = a_ray.start + a_ray.direction * inter.t;
    glm::vec3 inter_norm = glm::vec3(inter_point.x / a_box->size.x, inter_point.y / a_box->size.y, inter_point.z / a_box->size.z);
    float eps = 1e-3;
    if (std::abs(std::abs(inter_norm.x) - 1) <= eps) {
        inter.normal = {inter_norm.x, 0.f, 0.f};
    }
    else if (std::abs(std::abs(inter_norm.y) - 1) <= eps) {
        inter.normal = {0.f, inter_norm.y, 0.f};
    }
    else {
        inter.normal = {0.f, 0.f, inter_norm.z};
    }

    if (glm::dot(a_ray.direction, inter.normal) >= 0) {
        inter.inside = true;
        inter.normal *= -1;
    }
    return inter;
}

std::optional<Intersection> intersection(Ray a_ray, Shape* a_object) {
    a_ray.start -= a_object->position;
    glm::quat reversed_rotation = glm::inverse(a_object->rotation);
    a_ray.start = reversed_rotation * a_ray.start;
    a_ray.direction = glm::normalize(reversed_rotation * a_ray.direction);

    std::optional<Intersection> inter = std::nullopt;
    switch (a_object->type) {
    case PRIMITIVE_TYPE::Plane:
        inter = intersection(a_ray, dynamic_cast<Plane*>(a_object));
    case PRIMITIVE_TYPE::Ellipsoid:
        inter = intersection(a_ray, dynamic_cast<Ellipsoid*>(a_object));
    case PRIMITIVE_TYPE::Box:
        inter = intersection(a_ray, dynamic_cast<Box*>(a_object));
    default:
        throw std::runtime_error("Unknown primitive type");
    }

    if (!inter.has_value()) {
        return {};
    }
    inter->normal = glm::normalize(a_object->rotation * inter->normal);
    return inter;
}

glm::vec3 calc_diffuse_rawcolor(const Scene& scene, Shape* obj, const glm::vec3& inter_point, const Intersection& inter) {
    glm::vec3 result_color = scene.ambient_light;
    for (Light* light : scene.lights) {
        if (light->type == LIGHT_TYPE::Directed) {
            Ray ray{};
            ray.direction = light->direction;
            ray.start = inter_point;

            float dot_product = glm::dot(ray.direction, inter.normal);
            if (dot_product > 0) {
                result_color += obj->color * dot_product * light->intensity;
            }
        }
        else if (light->type == LIGHT_TYPE::Point) {
            Ray ray{};
            ray.direction = glm::normalize(light->position - inter_point);
            ray.start = inter_point;

            float dist = glm::distance(light->position, inter_point);
            glm::vec3 attenuated_intensity =
                light->intensity / (light->attenuation.x + light->attenuation.y * dist + light->attenuation.z * dist * dist);
            float dot_product = glm::dot(ray.direction, inter.normal);
            if (dot_product > 0) {
                result_color += obj->color * dot_product * attenuated_intensity;
            }
        }
    }
    return result_color;
}

Color calc_color(const Scene& scene, Shape* obj, const glm::vec3& inter_point, const Intersection& inter) {
    switch (obj->material) {
    case MATERIAL_TYPE::Diffuse:
        return color_converter(calc_diffuse_rawcolor(scene, obj, inter_point, inter));
    default:
        break;
    }
    return Color{};
}

std::pair<std::optional<float>, Color> raytrace(Ray& a_ray, const Scene& a_scene) {
    Color color = color_converter(a_scene.bg_color);
    std::optional<float> intersection_t = std::nullopt;

    for (auto primitive : a_scene.primitives) {
        auto intersection_result = intersection(a_ray, primitive);
        if (!intersection_result.has_value()) {
            continue;
        }
        if (!intersection_t.has_value() || intersection_t.value() > intersection_result->t) {
            intersection_t = intersection_result->t;
            glm::vec3 inter_point = a_ray.start + a_ray.direction * intersection_result->t;
            color = calc_color(a_scene, primitive, inter_point, intersection_result.value());
        }
    }
    return {intersection_t, color};
}

} // namespace engine::ray
