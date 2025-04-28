#pragma once

#include <optional>

#include "glm/gtc/quaternion.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

namespace engine {

namespace ray {
struct Ray;
}

enum class PRIMITIVE_TYPE { Plane, Ellipsoid, Box, Triangle };
enum class MATERIAL_TYPE { Metallic, Dielectric, Diffuse };

struct Intersection {
    Intersection();

    float t;
    glm::vec3 normal;
    bool inside;
};

struct Shape {
    Shape(PRIMITIVE_TYPE type);

    glm::vec3 color;
    glm::vec3 position;
    glm::quat rotation;
    PRIMITIVE_TYPE type;
    MATERIAL_TYPE material;
    glm::vec3 emission;
    float ior;

    virtual ~Shape() = default;
    virtual std::optional<Intersection> intersection(ray::Ray& ray) const = 0;
};

struct Plane : Shape {
    Plane();
    std::optional<Intersection> intersection(ray::Ray& ray) const final;

    glm::vec3 normal;
};

struct Ellipsoid : Shape {
    Ellipsoid();
    std::optional<Intersection> intersection(ray::Ray& ray) const final;

    glm::vec3 radius;
};

struct Box : Shape {
    Box();
    std::optional<Intersection> intersection(ray::Ray& ray) const final;

    glm::vec3 size;
};

struct Triangle : Shape {
    Triangle();
    std::optional<Intersection> intersection(ray::Ray& ray) const final;

    glm::vec3 vertex_1;
    glm::vec3 vertex_2;
    glm::vec3 vertex_3;
};

} // namespace engine
