#include "distributions.hpp"
#include "glm/geometric.hpp"
#include <random>

namespace engine::rand {

Uniform::Uniform()
    : norm(std::normal_distribution<float>(0.f, 1.f)) {}

glm::vec3 Uniform::sample(glm::vec3 x, glm::vec3 n) {
    glm::vec3 sample{
        norm(Rng::get_instance().gen()), 
        norm(Rng::get_instance().gen()), 
        norm(Rng::get_instance().gen())
    };
    sample = glm::normalize(sample);
    if (glm::dot(sample, n) < 0) {
        sample *= -1;
    }
    return sample;
}

float Uniform::pdf(glm::vec3 x, glm::vec3 n, glm::vec3 d) {
    if (glm::dot(d, n) < 0) {
        return 0.f;
    }
    return 1.f / (2.f * pi);
}

Cosine::Cosine()
    : norm(std::normal_distribution<float>(0.f, 1.f)) {}

glm::vec3 Cosine::sample(glm::vec3 x, glm::vec3 n) {
    glm::vec3 sample{
        norm(Rng::get_instance().gen()), 
        norm(Rng::get_instance().gen()), 
        norm(Rng::get_instance().gen())
    };
    sample = glm::normalize(sample);
    sample += n;
    // if (len < eps ...)
    return 1.f / glm::length(sample) * sample;
}

float Cosine::pdf(glm::vec3 x, glm::vec3 n, glm::vec3 d) {
    return std::max(0.f, glm::dot(d, d) / pi);
}

} // namespace engine::rand
