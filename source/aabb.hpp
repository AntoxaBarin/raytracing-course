#pragma once

#include "glm/vec3.hpp"
#include "primitive.hpp"

namespace engine {

struct AABB {
    AABB();
    AABB(glm::vec3 min, glm::vec3 max);
    AABB(Shape* obj);

    void extend(const glm::vec3& point);
    void extend(AABB aabb);
    void extend(Shape* obj);
    inline void extend(Box* box);
    inline void extend(Ellipsoid* ellips);
    inline void extend(Triangle* triangle);
    float surface_area() const;
    std::optional<Intersection> intersect(ray::Ray& ray) const;

    glm::vec3 min;
    glm::vec3 max;
};

} // namespace engine
