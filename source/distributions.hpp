#pragma once

#include <numbers>
#include <random>

#include "glm/ext/vector_float3.hpp"

namespace engine::rand {

constexpr float pi = std::numbers::pi_v<float>;

class Rng {
public:
    static Rng& get_instance() {
        static Rng instance;
        return instance;
    }

    std::minstd_rand& gen() {
        static std::random_device rd;
        static std::minstd_rand gen(rd());
        return gen;
    }

private:
    Rng() = default;
    Rng(const Rng&) = delete;
    Rng& operator=(const Rng&) = delete;
};

class IDistribution {
public:
    virtual ~IDistribution() = default;
    virtual glm::vec3 sample(glm::vec3 x, glm::vec3 n) = 0;
    virtual float pdf(glm::vec3 x, glm::vec3 n, glm::vec3 d) = 0;
};

class Uniform : public IDistribution {
public:
    Uniform();

    glm::vec3 sample(glm::vec3 x, glm::vec3 n) final;
    float pdf(glm::vec3 x, glm::vec3 n, glm::vec3 d) final;

private:
    std::normal_distribution<float> norm;
};

class Cosine : public IDistribution {
public:
    Cosine();

    glm::vec3 sample(glm::vec3 x, glm::vec3 n) final;
    float pdf(glm::vec3 x, glm::vec3 n, glm::vec3 d) final;

private:
    std::normal_distribution<float> norm;

};

class Light {};

class Mix {};

} // namespace engine::rand
