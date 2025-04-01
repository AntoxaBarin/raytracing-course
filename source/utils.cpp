#include "utils.hpp"

#include "glm/common.hpp"
#include "glm/exponential.hpp"
#include "glm/vec3.hpp"

#include <algorithm>
#include <cmath>
#include <random>

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

static std::random_device rd;
static std::minstd_rand gen(rd());

float rand_uniform(float a, float b) {
    static std::uniform_real_distribution<> rnd(0, 1);
    std::uniform_real_distribution<>::param_type bounds(a, b);
    rnd.param(bounds);
    return rnd(gen);
}

float rand_normal01() {
    static std::normal_distribution<> rnd(0, 1);
    return rnd(gen);
}

} // namespace engine
