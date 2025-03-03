#pragma once

#include "glm/gtc/quaternion.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

enum class PRIMITIVE_TYPE { Plane, Ellipsoid, Box };

struct Shape {
    Shape(PRIMITIVE_TYPE a_type);

    glm::vec3 color;
    glm::vec3 position;
    glm::quat rotation;
    PRIMITIVE_TYPE type;
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
