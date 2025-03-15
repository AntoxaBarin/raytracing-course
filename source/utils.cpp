#include "utils.hpp"

#include "glm/common.hpp"
#include "glm/exponential.hpp"
#include "glm/vec3.hpp"

#include <cmath>

namespace engine {

glm::vec3 saturate(const glm::vec3& color) { return glm::clamp(color, glm::vec3(0.0), glm::vec3(1.0)); }

glm::vec3 aces_tonemap(const glm::vec3& x) {
    const float a = 2.51f;
    const float b = 0.03f;
    const float c = 2.43f;
    const float d = 0.59f;
    const float e = 0.14f;
    return saturate((x * (a * x + b)) / (x * (c * x + d) + e));
}

glm::vec3 gamma_correction(const glm::vec3& color) { return glm::pow(color, glm::vec3{1.0 / 2.2}); }

ray::Color color_converter(glm::vec3 color) {
    color = aces_tonemap(color);
    color = gamma_correction(color);
    return {static_cast<uint8_t>(std::round(color.x * 255.0f)),
            static_cast<uint8_t>(std::round(color.y * 255.0f)),
            static_cast<uint8_t>(std::round(color.z * 255.0f))};
}

} // namespace engine
