#pragma once

#include "glm/gtc/quaternion.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

namespace engine {

enum class PRIMITIVE_TYPE { Plane, Ellipsoid, Box };
enum class MATERIAL_TYPE { Metallic, Dielectric, Diffuse };
enum class LIGHT_TYPE { Point, Directed };

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
};

struct Plane : Shape {
    Plane();

    glm::vec3 normal;
};

struct Ellipsoid : Shape {
    Ellipsoid();

    glm::vec3 radius;
};

struct Box : Shape {
    Box();

    glm::vec3 size;
};

struct Light {
    Light();

    glm::vec3 intensity;
    glm::vec3 direction;
    glm::vec3 position;
    glm::vec3 attenuation;
    LIGHT_TYPE type;
};

struct Intersection {
    Intersection();

    float t;
    glm::vec3 normal;
    bool inside;
};

} // namespace engine
