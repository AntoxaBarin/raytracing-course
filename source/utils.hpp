#pragma once

#include <cmath>
#include <cstdint>

#include "glm/vec3.hpp"
#include "ray.hpp"

namespace engine {

float tone_map(float in);
std::uint8_t color_converter(float in);

} // namespace engine
