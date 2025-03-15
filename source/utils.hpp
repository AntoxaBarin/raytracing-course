#pragma once

#include <cmath>
#include <cstdint>

#include "glm/vec3.hpp"
#include "ray.hpp"

namespace engine {

glm::vec3 saturate(const glm::vec3& color);
glm::vec3 aces_tonemap(const glm::vec3& x);
ray::Color color_converter(glm::vec3 color);

} // namespace engine
