#include "aabb.hpp"

#include "primitive.hpp"
#include "ray.hpp"

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

void AABB::extend(Shape* obj) {
    switch (obj->type) {
        case PRIMITIVE_TYPE::Box: {
            auto b = dynamic_cast<Box*>(obj);
            extend(b);
            break;
        }
        case PRIMITIVE_TYPE::Ellipsoid: {
            auto e = dynamic_cast<Ellipsoid*>(obj);
            extend(e);
            break;
        }
        case PRIMITIVE_TYPE::Triangle: {
            auto t = dynamic_cast<Triangle*>(obj);
            extend(t);
            break;
        }
        default:
            throw std::runtime_error("AABB: Unsupported shape type");
    }
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

std::optional<Intersection> AABB::intersect(ray::Ray& ray) const {
    auto s = 0.5f * (min + max);
    ray::Ray shifted_ray = ray;
    shifted_ray.start -= 0.5f * (min + max);

    glm::vec3 ts1 = (-1.f * s - shifted_ray.start) / shifted_ray.direction;
    glm::vec3 ts2 = (s - shifted_ray.start) / shifted_ray.direction;
    float tx_1 = std::min(ts1.x, ts2.x); 
    float tx_2 = std::max(ts1.x, ts2.x);
    
    float ty_1 = std::min(ts1.y, ts2.y); 
    float ty_2 = std::max(ts1.y, ts2.y);
    
    float tz_1 = std::min(ts1.z, ts2.z); 
    float tz_2 = std::max(ts1.z, ts2.z);
    
    float t1 = std::max(std::max(tx_1, ty_1), tz_1);
    float t2 = std::min(std::min(tx_2, ty_2), tz_2);
    
    if (t1 > t2 || t2 < 0) {
        return {};
    }
    
    float t;
    bool is_inside;
    if (t1 < 0) {
        is_inside = true;
        t = t2;
    } else {
        is_inside = false;
        t = t1;
    }

    glm::vec3 p = shifted_ray.start + t * shifted_ray.direction;
    glm::vec3 normal = p / s;
    float mx = std::max(std::max(std::abs(normal.x), std::abs(normal.y)), std::abs(normal.z));

    if (fabs(normal.x) != mx) {
        normal.x = 0;
    }
    if (fabs(normal.y) != mx) {
        normal.y = 0;
    }
    if (fabs(normal.z) != mx) {
        normal.z = 0;
    }

    if (is_inside) {
        normal = -1.f * normal;
    }

    Intersection inter{};
    inter.t = t;
    inter.normal = glm::normalize(normal);
    inter.inside = is_inside;

    return inter;
}

} // namespace engine
