#include "ray.hpp"
#include "glm/geometric.hpp"
#include "primitive.hpp"
#include "utils.hpp"
#include "distributions.hpp"

#include <cmath>
#include <cstdint>
#include <optional>
#include <stdexcept>

namespace engine::ray {

Ray generate_ray(const Scene& scene, std::pair<std::uint32_t, std::uint32_t> pixel_coord) {
    Ray ray{};
    ray.start = scene.camera.camera_position;

    float x = static_cast<float>(pixel_coord.first) + rand_uniform01();
    float y = static_cast<float>(pixel_coord.second) + rand_uniform01();

    x = (2.f * x / static_cast<float>(scene.width) - 1) * tanf(scene.camera.camera_fov_x / 2);
    y = -(2.f * y / static_cast<float>(scene.height) - 1) *
        (tanf(scene.camera.camera_fov_x / 2) / (static_cast<float>(scene.width) / scene.height));
    ray.direction = glm::normalize(x * scene.camera.camera_right + y * scene.camera.camera_up + scene.camera.camera_forward);
    return ray;
}

std::optional<Intersection> intersection(Ray ray, Shape* object) {
    ray.start -= object->position;
    glm::quat reversed_rotation = glm::inverse(object->rotation);
    ray.start = reversed_rotation * ray.start;
    ray.direction = glm::normalize(reversed_rotation * ray.direction);

    std::optional<Intersection> inter = object->intersection(ray);
    if (!inter.has_value()) {
        return {};
    }
    inter->normal = glm::normalize(object->rotation * inter->normal);
    return inter;
}

glm::vec3 calc_diffuse_rawcolor(const Scene& scene, Shape* obj, Ray in_ray, const Intersection& inter, std::uint32_t ray_depth) {
    glm::vec3 result_color = obj->color;
    float eps = 1e-4;
    glm::vec3 inter_point = in_ray.start + in_ray.direction * inter.t;

    auto rnd_dir = scene.distribution->sample(inter_point + eps * inter.normal, inter.normal);
    if (glm::dot(rnd_dir, inter.normal) < 0) {
        return obj->emission;
    }
    float pdf = scene.distribution->pdf(inter_point + eps * inter.normal, inter.normal, rnd_dir);

    Ray out_ray{};
    out_ray.direction = glm::normalize(rnd_dir);
    if (glm::dot(out_ray.direction, inter.normal) < 0) {
        out_ray.direction *= -1;
    }
    out_ray.start = inter_point + out_ray.direction * eps;

    glm::vec3 color = raytrace(out_ray, scene, ray_depth + 1).second;
    return obj->emission + (1.f / pdf) * obj->color / rand::pi * color * glm::dot(out_ray.direction, inter.normal);
}

glm::vec3 calc_metallic_rawcolor(const Scene& scene, Shape* obj, Ray in_ray, const Intersection& inter, std::uint32_t ray_depth) {
    float eps = 1e-4;
    glm::vec3 inter_point = in_ray.start + in_ray.direction * inter.t;
    Ray reflected_ray{};
    reflected_ray.direction = in_ray.direction - 2.f * inter.normal * glm::dot(inter.normal, in_ray.direction);
    reflected_ray.start = inter_point + reflected_ray.direction * eps;

    auto next_raytrace = raytrace(reflected_ray, scene, ray_depth + 1);
    return obj->emission + obj->color * next_raytrace.second;
}

glm::vec3 calc_dielectric_rawcolor(const Scene& scene, Shape* obj, Ray in_ray, const Intersection& inter, std::uint32_t ray_depth) {
    float eps = 1e-4;
    glm::vec3 inter_point = in_ray.start + in_ray.direction * inter.t;

    float cos_theta1 = glm::dot(-in_ray.direction, inter.normal);
    float air_ior = 1.f;
    float obj_ior = obj->ior;
    if (inter.inside) {
        std::swap(air_ior, obj_ior);
    }
    float reflection_coef = std::pow((air_ior - obj_ior) / (air_ior + obj_ior), 2);
    float reflected_light = reflection_coef + (1 - reflection_coef) * std::pow(1 - cos_theta1, 5);
    float sin_theta2 = (air_ior / obj_ior) * sqrt(1 - cos_theta1 * cos_theta1);

    float coin_toss = rand_uniform01();
    if (std::abs(sin_theta2) > 1 || coin_toss < reflection_coef) {
        Ray reflected_ray{};
        reflected_ray.direction = in_ray.direction - 2.f * inter.normal * glm::dot(inter.normal, in_ray.direction);
        reflected_ray.start = inter_point + reflected_ray.direction * eps;
        glm::vec3 reflected_color = raytrace(reflected_ray, scene, ray_depth + 1).second;

        if (inter.inside) {
            return reflected_color;
        }
        return obj->emission + reflected_color;
    }

    float cos_theta2 = sqrt(1 - sin_theta2 * sin_theta2);
    Ray refracted_ray{};
    refracted_ray.direction = (air_ior / obj_ior) * in_ray.direction + (air_ior / obj_ior * cos_theta1 - cos_theta2) * inter.normal;
    refracted_ray.start = inter_point + refracted_ray.direction * eps;
    glm::vec3 refracted_color = raytrace(refracted_ray, scene, ray_depth + 1).second;

    if (inter.inside) {
        return refracted_color;
    }
    return obj->emission + refracted_color * obj->color;
}

glm::vec3 calc_color(const Scene& scene, Shape* obj, Ray ray, const Intersection& inter, std::uint32_t ray_depth) {
    switch (obj->material) {
    case MATERIAL_TYPE::Diffuse:
        return calc_diffuse_rawcolor(scene, obj, ray, inter, ray_depth);
    case MATERIAL_TYPE::Metallic:
        return calc_metallic_rawcolor(scene, obj, ray, inter, ray_depth);
    case MATERIAL_TYPE::Dielectric:
        return calc_dielectric_rawcolor(scene, obj, ray, inter, ray_depth);
    default:
        throw std::runtime_error("Unknown material type.");
    }
    return glm::vec3{};
}

std::pair<std::optional<float>, glm::vec3> raytrace(Ray& ray, const Scene& scene, std::uint32_t ray_depth) {
    glm::vec3 color = scene.bg_color;
    std::optional<float> inter_t{std::nullopt};
    std::optional<Intersection> closest_inter{std::nullopt};
    Shape* closest_primitive = nullptr;

    if (ray_depth == scene.ray_depth) {
        return {inter_t, color};
    }

    for (uint32_t i = scene.first_plane_idx; i < scene.primitives.size(); ++i) {
        auto cur_inter = intersection(ray, scene.primitives[i]);
        if (!cur_inter.has_value()) {
            continue;
        }
        if (!inter_t.has_value() || inter_t.value() > cur_inter->t) {
            inter_t = cur_inter->t;
            closest_inter = cur_inter;
            closest_primitive = scene.primitives[i];
        }
    }

    auto bvh_inter = scene.bvh.intersect(scene.primitives, scene.bvh.root_idx, ray, inter_t);
    if (bvh_inter.has_value() && (!closest_inter.has_value() || bvh_inter.value().first.t < closest_inter->t)) {
        inter_t = bvh_inter.value().first.t;
        closest_inter = bvh_inter.value().first;
        closest_primitive = scene.primitives[bvh_inter.value().second];
    }

    if (closest_inter.has_value()) {
        color = calc_color(scene, closest_primitive, ray, closest_inter.value(), ray_depth);
    }
    return {inter_t, color};
}

} // namespace engine::ray
