#include "utils.hpp"

#include "glm/common.hpp"
#include "glm/exponential.hpp"
#include "glm/vec3.hpp"

#include <algorithm>
#include <cmath>

namespace engine {

float tone_map(float in) {
    const float a = 2.51f;
    const float b = 0.03f;
    const float c = 2.43f;
    const float d = 0.59f;
    const float e = 0.14f;
    float color = (in * (a * in + b)) / (in * (c * in + d) + e);
    return glm::clamp(color, 0.f, 1.f);
}

std::uint8_t color_converter(float in) {
    in = tone_map(in);
    in = std::pow(in, 1.0 / 2.2);
    return std::round(std::clamp(in * 255, 0.f, 255.f));
}

} // namespace engine
