#include "primitive.hpp"

namespace engine {

Shape::Shape(PRIMITIVE_TYPE type)
    : type(type),
      color({0.f, 0.f, 0.f}),
      position({0.f, 0.f, 0.f}),
      rotation({1.f, 0.f, 0.f, 0.f}),
      material(MATERIAL_TYPE::Diffuse),
      ior(1.f) {}

Plane::Plane()
    : Shape(PRIMITIVE_TYPE::Plane),
      normal({0.f, 0.f, 0.f}) {}

Ellipsoid::Ellipsoid()
    : Shape(PRIMITIVE_TYPE::Ellipsoid),
      radius({0.f, 0.f, 0.f}) {}

Box::Box()
    : Shape(PRIMITIVE_TYPE::Box),
      size({0.f, 0.f, 0.f}) {}

Light::Light()
    : intensity({0.f, 0.f, 0.f}),
      direction({0.f, 0.f, 0.f}),
      position({0.f, 0.f, 0.f}),
      attenuation({0.f, 0.f, 0.f}),
      type(LIGHT_TYPE::Point) {}

Intersection::Intersection()
    : t(0.f),
      normal({0.f, 0.f, 0.f}),
      inside(false) {}

} // namespace engine
