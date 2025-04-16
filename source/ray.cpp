#include "ray.hpp"
#include "glm/geometric.hpp"
#include "primitive.hpp"
#include "utils.hpp"
#include "distributions.hpp"

#include <cmath>
#include <limits>
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

std::optional<Intersection> intersection(Ray& ray, Plane* plane) {
    Intersection inter{};
    inter.t = -glm::dot(ray.start, plane->normal) / glm::dot(ray.direction, plane->normal);
    if (inter.t < 0) {
        return {};
    }
    inter.normal = plane->normal;
    if (glm::dot(ray.direction, plane->normal) >= 0) {
        inter.inside = true;
        // inter.normal *= 1;
    }
    return inter;
}

std::optional<Intersection> intersection(Ray& ray, Ellipsoid* ellips) {
    Intersection inter{};
    inter.normal = glm::vec3(1.0f);
    float a =
        glm::dot(glm::vec3{ray.direction.x / ellips->radius.x, ray.direction.y / ellips->radius.y, ray.direction.z / ellips->radius.z},
                 glm::vec3{ray.direction.x / ellips->radius.x, ray.direction.y / ellips->radius.y, ray.direction.z / ellips->radius.z});

    float b =
        glm::dot(glm::vec3{ray.start.x / ellips->radius.x, ray.start.y / ellips->radius.y, ray.start.z / ellips->radius.z},
                 glm::vec3{ray.direction.x / ellips->radius.x, ray.direction.y / ellips->radius.y, ray.direction.z / ellips->radius.z});

    float c = glm::dot(glm::vec3{ray.start.x / ellips->radius.x, ray.start.y / ellips->radius.y, ray.start.z / ellips->radius.z},
                       glm::vec3{ray.start.x / ellips->radius.x, ray.start.y / ellips->radius.y, ray.start.z / ellips->radius.z});
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

    glm::vec3 inter_point = ray.start + ray.direction * inter.t;
    glm::vec3 inter_norm = glm::vec3(inter_point.x / (ellips->radius.x * ellips->radius.x),
                                     inter_point.y / (ellips->radius.y * ellips->radius.y),
                                     inter_point.z / (ellips->radius.z * ellips->radius.z));
    inter.normal = glm::normalize(inter_norm);

    if (glm::dot(ray.direction, inter.normal) >= 0) {
        inter.inside = true;
        inter.normal *= -1;
    }
    return inter;
}

std::optional<Intersection> intersection(Ray& ray, Box* box) {
    Intersection inter{};
    inter.normal = glm::vec3(1.0f);

    float tx_1 = (box->size.x - ray.start.x) / ray.direction.x;
    float tx_2 = (-box->size.x - ray.start.x) / ray.direction.x;
    if (tx_2 < tx_1) {
        std::swap(tx_1, tx_2);
    }
    float ty_1 = (box->size.y - ray.start.y) / ray.direction.y;
    float ty_2 = (-box->size.y - ray.start.y) / ray.direction.y;
    if (ty_2 < ty_1) {
        std::swap(ty_1, ty_2);
    }
    float tz_1 = (box->size.z - ray.start.z) / ray.direction.z;
    float tz_2 = (-box->size.z - ray.start.z) / ray.direction.z;
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

    glm::vec3 inter_point = ray.start + ray.direction * inter.t;
    glm::vec3 inter_norm = glm::vec3(inter_point.x / box->size.x, inter_point.y / box->size.y, inter_point.z / box->size.z);
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

    if (glm::dot(ray.direction, inter.normal) >= 0) {
        inter.inside = true;
        inter.normal *= -1;
    }
    return inter;
}

std::optional<Intersection> intersection(Ray ray, Shape* object) {
    ray.start -= object->position;
    glm::quat reversed_rotation = glm::inverse(object->rotation);
    ray.start = reversed_rotation * ray.start;
    ray.direction = glm::normalize(reversed_rotation * ray.direction);

    std::optional<Intersection> inter = std::nullopt;
    switch (object->type) {
    case PRIMITIVE_TYPE::Plane:
        inter = intersection(ray, dynamic_cast<Plane*>(object));
        break;
    case PRIMITIVE_TYPE::Ellipsoid:
        inter = intersection(ray, dynamic_cast<Ellipsoid*>(object));
        break;
    case PRIMITIVE_TYPE::Box:
        inter = intersection(ray, dynamic_cast<Box*>(object));
        break;
    default:
        throw std::runtime_error("Unknown primitive type");
    }

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
    
    rand::Cosine cosine_sampler{};
    glm::vec3 rnd_dir = cosine_sampler.sample({}, inter.normal);
    float pdf = cosine_sampler.pdf({}, inter.normal, rnd_dir);

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

    for (auto primitive : scene.primitives) {
        auto cur_inter = intersection(ray, primitive);
        if (!cur_inter.has_value()) {
            continue;
        }
        if (!inter_t.has_value() || inter_t.value() > cur_inter->t) {
            inter_t = cur_inter->t;
            closest_inter = cur_inter;
            closest_primitive = primitive;
        }
    }
    if (closest_inter.has_value()) {
        color = calc_color(scene, closest_primitive, ray, closest_inter.value(), ray_depth);
    }
    return {inter_t, color};
}

} // namespace engine::ray
