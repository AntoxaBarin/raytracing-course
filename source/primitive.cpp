#include "primitive.hpp"

#include "ray.hpp"

namespace engine {

Shape::Shape(PRIMITIVE_TYPE type)
    : type(type),
      color({0.f, 0.f, 0.f}),
      position({0.f, 0.f, 0.f}),
      rotation({1.f, 0.f, 0.f, 0.f}),
      material(MATERIAL_TYPE::Diffuse),
      emission(0.f, 0.f, 0.f),
      ior(1.f) {}

Plane::Plane()
    : Shape(PRIMITIVE_TYPE::Plane),
      normal({0.f, 0.f, 0.f}) {}

std::optional<Intersection> Plane::intersection(ray::Ray& ray) const {
    Intersection inter{};
    inter.t = -glm::dot(ray.start, normal) / glm::dot(ray.direction, normal);
    if (inter.t < 0) {
        return {};
    }
    inter.normal = normal;
    if (glm::dot(ray.direction, normal) >= 0) {
        inter.inside = true;
    }
    return inter;
}

Ellipsoid::Ellipsoid()
    : Shape(PRIMITIVE_TYPE::Ellipsoid),
      radius({0.f, 0.f, 0.f}) {}

std::optional<Intersection> Ellipsoid::intersection(ray::Ray& ray) const {
    Intersection inter{};
    inter.normal = glm::vec3(1.0f);
    float a = glm::dot(glm::vec3{ray.direction.x / radius.x, ray.direction.y / radius.y, ray.direction.z / radius.z},
                       glm::vec3{ray.direction.x / radius.x, ray.direction.y / radius.y, ray.direction.z / radius.z});

    float b = glm::dot(glm::vec3{ray.start.x / radius.x, ray.start.y / radius.y, ray.start.z / radius.z},
                       glm::vec3{ray.direction.x / radius.x, ray.direction.y / radius.y, ray.direction.z / radius.z});

    float c = glm::dot(glm::vec3{ray.start.x / radius.x, ray.start.y / radius.y, ray.start.z / radius.z},
                       glm::vec3{ray.start.x / radius.x, ray.start.y / radius.y, ray.start.z / radius.z});
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
    glm::vec3 inter_norm = glm::vec3(inter_point.x / (radius.x * radius.x),
                                     inter_point.y / (radius.y * radius.y),
                                     inter_point.z / (radius.z * radius.z));
    inter.normal = glm::normalize(inter_norm);

    if (glm::dot(ray.direction, inter.normal) >= 0) {
        inter.inside = true;
        inter.normal *= -1;
    }
    return inter;
}

Box::Box()
    : Shape(PRIMITIVE_TYPE::Box),
      size({0.f, 0.f, 0.f}) {}

std::optional<Intersection> Box::intersection(ray::Ray& ray) const {
    Intersection inter{};
    inter.normal = glm::vec3(1.0f);

    float tx_1 = (size.x - ray.start.x) / ray.direction.x;
    float tx_2 = (-size.x - ray.start.x) / ray.direction.x;
    if (tx_2 < tx_1) {
        std::swap(tx_1, tx_2);
    }
    float ty_1 = (size.y - ray.start.y) / ray.direction.y;
    float ty_2 = (-size.y - ray.start.y) / ray.direction.y;
    if (ty_2 < ty_1) {
        std::swap(ty_1, ty_2);
    }
    float tz_1 = (size.z - ray.start.z) / ray.direction.z;
    float tz_2 = (-size.z - ray.start.z) / ray.direction.z;
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
    glm::vec3 inter_norm = glm::vec3(inter_point.x / size.x, inter_point.y / size.y, inter_point.z / size.z);
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

Triangle::Triangle()
    : Shape(PRIMITIVE_TYPE::Triangle) {}

std::optional<Intersection> Triangle::intersection(ray::Ray& ray) const { return {}; }

Intersection::Intersection()
    : t(0.f),
      normal({0.f, 0.f, 0.f}),
      inside(false) {}

} // namespace engine
