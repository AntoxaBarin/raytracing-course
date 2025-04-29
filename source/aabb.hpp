#pragma once

#include "glm/vec3.hpp"
#include "primitive.hpp"

namespace engine {

struct AABB {
    AABB();
    AABB(glm::vec3 min, glm::vec3 max);
    AABB(Shape* obj);

    void resize(const glm::vec3& point);
    void resize(Box* box);
    void resize(Ellipsoid* ellips);
    void resize(Triangle* triangle);

    glm::vec3 min;
    glm::vec3 max;
};

} // namespace engine
