#include "primitive.hpp"

shape::shape(PRIMITIVE_TYPE a_type) : type(a_type) {
    color = {0.f, 0.f, 0.f};
    position = {0.f, 0.f, 0.f};
    rotation = {1.f, 0.f, 0.f, 0.f};
}

plane::plane() : shape(PRIMITIVE_TYPE::Plane) {
    normal = {0.f, 0.f, 0.f};
}

ellipsoid::ellipsoid() : shape(PRIMITIVE_TYPE::Ellipsoid) {
    radius = {0.f, 0.f, 0.f};
}

box::box() : shape(PRIMITIVE_TYPE::Box) {
    size = {0.f, 0.f, 0.f};
}
