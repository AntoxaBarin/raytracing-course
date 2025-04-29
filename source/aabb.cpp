#include "aabb.hpp"

#include "primitive.hpp"

#include <cassert>
#include <stdexcept>

namespace engine {

AABB::AABB()
    : min(glm::vec3(0.0f)),
      max(glm::vec3(0.0f)) {}

AABB::AABB(glm::vec3 min, glm::vec3 max)
    : min(min),
      max(max) {}

AABB::AABB(Shape* obj) {
    if (obj->type == PRIMITIVE_TYPE::Box) {
        auto b = dynamic_cast<Box*>(obj);
        extend(b);
    }
    else if (obj->type == PRIMITIVE_TYPE::Ellipsoid) {
        auto e = dynamic_cast<Ellipsoid*>(obj);
        extend(e);
    }
    else if (obj->type == PRIMITIVE_TYPE::Triangle) {
        auto t = dynamic_cast<Triangle*>(obj);
        extend(t);
    }   
    else {
        throw std::runtime_error("AABB: Unsupported shape type");
    }

    if (obj->rotation == glm::quat(1.0f, 0.0f, 0.0f, 0.0f)) {
        min += obj->position;
        max += obj->position;
        return;
    }
    glm::quat rotation = glm::conjugate(obj->rotation);
    AABB not_rotated_aabb = *this;
    min = rotation * glm::vec3(not_rotated_aabb.min);
    max = rotation * glm::vec3(not_rotated_aabb.min);
    
    extend(rotation * glm::vec3(not_rotated_aabb.min.x, not_rotated_aabb.min.y, not_rotated_aabb.max.z));
    extend(rotation * glm::vec3(not_rotated_aabb.min.x, not_rotated_aabb.max.y, not_rotated_aabb.min.z));
    extend(rotation * glm::vec3(not_rotated_aabb.min.x, not_rotated_aabb.max.y, not_rotated_aabb.max.z));
    extend(rotation * glm::vec3(not_rotated_aabb.max.x, not_rotated_aabb.min.y, not_rotated_aabb.min.z));
    extend(rotation * glm::vec3(not_rotated_aabb.max.x, not_rotated_aabb.min.y, not_rotated_aabb.max.z));
    extend(rotation * glm::vec3(not_rotated_aabb.max.x, not_rotated_aabb.max.y, not_rotated_aabb.min.z));
    extend(rotation * glm::vec3(not_rotated_aabb.max.x, not_rotated_aabb.max.y, not_rotated_aabb.max.z));
    
    min = min + obj->position;
    max = max + obj->position;
}

void AABB::extend(const glm::vec3& point) {
    min.x = std::min(min.x, point.x);
    min.y = std::min(min.y, point.y);
    min.z = std::min(min.z, point.z);
    max.x = std::max(max.x, point.x);
    max.y = std::max(max.y, point.y);
    max.z = std::max(max.z, point.z);
}

inline void AABB::extend(AABB aabb) {
    min = glm::min(min, aabb.min);
    max = glm::max(max, aabb.max);
}

inline void AABB::extend(Box* box) {
    min = glm::min(min, -box->size);
    max = glm::max(max, box->size);
}

inline void AABB::extend(Ellipsoid* ellips) {
    min = glm::min(min, -ellips->radius);
    max = glm::max(max, ellips->radius);
}
    
void AABB::extend(Triangle* triangle) {
    glm::vec3 a = triangle->vertex_1;
    glm::vec3 b = triangle->vertex_2;
    glm::vec3 c = triangle->vertex_3;

    auto tr_min = glm::vec3{
        std::min(a.x, std::min(b.x, c.x)),
        std::min(a.y, std::min(b.y, c.y)),
        std::min(a.z, std::min(b.z, c.z))
    };
    auto tr_max = glm::vec3{
        std::max(a.x, std::max(b.x, c.x)),
        std::max(a.y, std::max(b.y, c.y)),
        std::max(a.z, std::max(b.z, c.z))
    };

    min = glm::min(min, tr_min);
    max = glm::max(max, tr_max);
}

float AABB::surface_area() const {
    float x = max.x - min.x;
    float y = max.y - min.y;
    float z = max.z - min.z;
    return 2.f * (x * y + x * z + y * z);
}

} // namespace engine
