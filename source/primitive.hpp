#pragma once

#include "glm/gtc/quaternion.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

enum class PRIMITIVE_TYPE {
    Plane,
    Ellipsoid,
    Box
};

struct shape {
    shape(PRIMITIVE_TYPE type);

    glm::vec3 color;
    glm::vec3 position;
    glm::quat rotation;
    PRIMITIVE_TYPE type;
};

struct plane : shape {
    plane();
    
    glm::vec3 normal;
};

struct ellipsoid : shape {
    ellipsoid();

    glm::vec3 radius;
};

struct box : shape {
    box();

    glm::vec3 size; 
};
