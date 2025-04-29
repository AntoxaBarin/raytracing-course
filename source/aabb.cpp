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
        resize(b);
    }
    else if (obj->type == PRIMITIVE_TYPE::Ellipsoid) {
        auto e = dynamic_cast<Ellipsoid*>(obj);
        resize(e);
    }
    else if (obj->type == PRIMITIVE_TYPE::Triangle) {
        auto t = dynamic_cast<Triangle*>(obj);
        resize(t);
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
    
    resize(rotation * glm::vec3(not_rotated_aabb.min.x, not_rotated_aabb.min.y, not_rotated_aabb.max.z));
    resize(rotation * glm::vec3(not_rotated_aabb.min.x, not_rotated_aabb.max.y, not_rotated_aabb.min.z));
    resize(rotation * glm::vec3(not_rotated_aabb.min.x, not_rotated_aabb.max.y, not_rotated_aabb.max.z));
    resize(rotation * glm::vec3(not_rotated_aabb.max.x, not_rotated_aabb.min.y, not_rotated_aabb.min.z));
    resize(rotation * glm::vec3(not_rotated_aabb.max.x, not_rotated_aabb.min.y, not_rotated_aabb.max.z));
    resize(rotation * glm::vec3(not_rotated_aabb.max.x, not_rotated_aabb.max.y, not_rotated_aabb.min.z));
    resize(rotation * glm::vec3(not_rotated_aabb.max.x, not_rotated_aabb.max.y, not_rotated_aabb.max.z));
    
    min = min + obj->position;
    max = max + obj->position;

}

void AABB::resize(const glm::vec3& point) {
    min.x = std::min(min.x, point.x);
    min.y = std::min(min.y, point.y);
    min.z = std::min(min.z, point.z);
    max.x = std::max(max.x, point.x);
    max.y = std::max(max.y, point.y);
    max.z = std::max(max.z, point.z);
}

void AABB::resize(Box* box) {
    glm::vec3 box_min = -box->size;
    glm::vec3 box_max = box->size;
    min = glm::min(min, box_min);
    max = glm::max(max, box_max);
}

void AABB::resize(Ellipsoid* ellips) {
    glm::vec3 el_min = -ellips->radius;
    glm::vec3 el_max = ellips->radius;
    min = glm::min(min, el_min);
    max = glm::max(max, el_max);
}
    
void AABB::resize(Triangle* triangle) {
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

} // namespace engine
