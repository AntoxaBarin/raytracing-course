#pragma once

#include <cmath>
#include <cstdint>

namespace engine {

float tone_map(float in);
std::uint8_t color_converter(float in);

float rand_uniform01();
float rand_normal01();

} // namespace engine
